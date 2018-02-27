/* This file is part of Insight.
 * Copyright (C) 2017-2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
 *
 * Insight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Insight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Insight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "GraphicEngine.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "ShellInterpreter.hpp"
#include "World.hpp"

/**
 * Represents possible states of the Insight class.
 *
 * Enables the control thread (Lua shell) to control worker threads (computing the
 * simulation).
 */
class InsightState {
private:
    /** Enumeration of all the possible states. */
    enum class State {
        stopped,  /**< Nothing is running. */
        running,  /**< Simulation is running. */
        pausing,  /**< Worker threads requested to sleep. */
        paused,   /**< Simulation on pause. */
        stopping  /**< Worker threads requested to terminate. */
    };

    /** State of this object. */
    std::atomic<State> state;
    /** Mutex protecting state for RMW operations (read operations can be done directly to state). */
    std::mutex mutex;
    /** Condition variable used to pause worker threads. */
    std::condition_variable workerCondition;
    /** Condition variable used by control thread to wait for worker threads. */
    std::condition_variable controlCondition;
public:
    /**
     * Creates a new InsightState in stopped mode.
     */
    InsightState() : state(State::stopped) {

    }

    /**
     * Transition from state State::stopped to State::running.
     */
    void boot() {
        std::lock_guard<std::mutex> lock(mutex);
        if (state != State::stopped) {
            throw std::logic_error("Cannot boot: object is not 'stopped'.");
        }
        state = State::running;
    }

    /**
     * Send a command to terminate worker threads.
     *
     * Possible only from State::paused and State::running.
     */
    void stopping() {
        std::unique_lock<std::mutex> lock(mutex);
        switch(state) {
        case State::stopped:
            break;
        case State::paused:
        case State::running:
            state = State::stopping;
            workerCondition.notify_all();
            break;
        default:
            throw std::logic_error("Cannot request stop: object is not 'paused' or 'running'.");
        }
    }

    /**
     * Transition from State::stopping to State::stopped.
     *
     * The caller must ensure that all threads have terminated by joining them.
     */
    void stop() {
        std::lock_guard<std::mutex> lock(mutex);
        if (state != State::stopping) {
            throw std::logic_error("Cannot stop: object is not 'stopping'.");
        }
        state = State::stopped;
    }

    /**
     * Sends a command to the worker thread to resume the simulation.
     *
     * Possible only from State::paused.
     */
    void resume() {
        std::unique_lock<std::mutex> lock(mutex);
        switch(state) {
        case State::running:
            break;
        case State::paused:
            state = State::running;
            lock.unlock();
            workerCondition.notify_all();
            break;
        default:
            throw std::logic_error("Cannot run: object not in 'paused' or 'running' mode.");
        }
    }

    /**
     * Sends a command to the worker thread to pause the simulation.
     *
     * Possible only from State::running or State:: pausing.
     */
    void pause() {
        std::unique_lock<std::mutex> lock(mutex);
        switch (state) {
        case State::paused:
            break;
        case State::running:
            state = State::pausing;
            // no break
        case State::pausing:
            controlCondition.wait(lock, [this]() -> bool { return this->state != State::pausing; });
            break;
        default:
            throw std::logic_error("Cannot pause: object is not in 'paused', 'pausing' or 'running' state.");
        }
    }

    /**
     * Wait to be in a non-paused state, then tests for running state.
     *
     * If the state is pausing, the calling thread will sleep until the state
     * changes. Then, it will return true if the application is in a 'running' state,
     * and false in other cases.
     *
     * @return True if the state is 'running'.
     */
    bool waitRunningState() {
        State readState = state;
        if (readState == State::pausing) {
            std::unique_lock<std::mutex> lock(mutex);
            state = State::paused;
            controlCondition.notify_all();
            workerCondition.wait(lock, [this]() -> bool { return this->state != State::paused;} );
            readState = state;
        }
        return (readState == State::running);
    }
};

class Insight : public LuaVirtualClass {
private:
    using timer = std::chrono::steady_clock;

    World world;
    GraphicEngine graphicEngine;
    ShellInterpreter interpreter;

    std::chrono::duration<std::int64_t, std::nano> renderPeriod;

    /** Holds the state of this object, and handles worker thread control (stop & pause). */
    InsightState insightState;


    /**
     * Worker thread main loop : computes the simulation.
     *
     * Other threads (Lua interpreter thread) can pause or stop this loop using
     * insightState.
     */
    void workerMainLoop() {
        while (insightState.waitRunningState() && graphicEngine.run()) {
            auto start = timer::now();
            world.stepSimulation(std::chrono::duration<double>(renderPeriod).count());
            graphicEngine.doRender();
            auto ellapsed = timer::now() - start;

            if (ellapsed < renderPeriod) {
                auto sleepTime = renderPeriod - ellapsed;
                std::this_thread::sleep_for(sleepTime);
            }
        }
    }
public:

    /**
     * Constructs a new Insight object.
     *
     * The run() method must be called to launch the different components (simulation, Lua shell).
     */
    Insight() : graphicEngine(world), interpreter(*this, "insight"), renderPeriod(std::chrono::nanoseconds(1000000000/60)) {

    }

    /**
     * Stops the current run.
     *
     * A run is started when the run() method is called.
     */
    void quit() {
        insightState.stopping();
        interpreter.quit();
    }

    /**
     * Starts the different components (Lua interpreter, simulation).
     *
     * This function will return when quit() has been called and all components
     * are stopped.
     */
    void run() {
        insightState.boot();
        std::thread shellThread([this]() { this->interpreter.run(); });
        workerMainLoop();

        shellThread.join();
        insightState.stop();
    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override {
        if (memberName == "world") {
            state.push<World*>(&world);
            return 1;
        } else if (memberName == "quit") {
            state.push<LuaMethod<Insight>>([](Insight& object, LuaStateView& state) -> int {
                object.quit();
                return 0;
            });
            return 1;
        }
        return 0;
    }

};

/*
 *
 */
int main(int argc, char** argv) {
    Insight insight;

    insight.run();

    return EXIT_SUCCESS;
}


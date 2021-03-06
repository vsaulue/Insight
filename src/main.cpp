/* This file is part of Insight.
 * Copyright (C) 2017-2019 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_set>

#include <boost/dll.hpp>
#include <boost/program_options.hpp>

#include "AI.hpp"
#include "AIFactory.hpp"
#include "GraphicEngine.hpp"
#include "lua/bindings/AIs.hpp"
#include "lua/bindings/insight.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/bindings/robotics.hpp"
#include "lua/bindings/std/shared_ptr.hpp"
#include "lua/types/LuaFunction.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "Robot.hpp"
#include "RobotBody.hpp"
#include "ShellInterpreter.hpp"
#include "ShellInterpreterConfig.hpp"
#include "version.hpp"
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
     *
     * @param[in] sleepUntilPaused True to wait until the worker thread stopped. False to just send a pause signal.
     * @return True if this call did pause the threads (they were running before this call).
     */
    bool pause(bool sleepUntilPaused) {
        std::unique_lock<std::mutex> lock(mutex);
        bool result = false;
        switch (state) {
        case State::paused:
            break;
        case State::running:
            state = State::pausing;
            result = true;
            // no break
        case State::pausing:
            if (sleepUntilPaused) {
                controlCondition.wait(lock, [this]() -> bool { return this->state != State::pausing; });
            }
            break;
        default:
            throw std::logic_error("Cannot pause: object is not in 'paused', 'pausing' or 'running' state.");
        }
        return result;
    }

    /**
     * Tests if this object is in paused state.
     * @return True if paused, false otherwise.
     */
    bool isPaused() {
        return (state==State::paused);
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

/** State of the simulation in the Insight class. */
class SimulationState {
public:
    /**
     * Creates a new SimulationState, paused by default.
     */
    SimulationState() : state(State::paused) {

    }

    /**
     * Transition from State::paused to State::running.
     */
    void resume() {
        std::lock_guard<std::mutex> lock(mutex);
        switch(state) {
        case State::running:
            break;
        case State::paused:
            state = State::running;
            break;
        default:
            throw std::logic_error("Cannot resume: object is not in 'paused' state.");
        }
    }

    /**
     * Transition from State::running to State::paused.
     *
     * Sends a pause command to the worker thread (using State::pausing).
     * Optionally wait for an acknowledgment from the worker thread.
     *
     * @param[in] sleepUntilPaused Wait until the worker thread has actually stopped.
     */
    void pause(bool sleepUntilPaused) {
        std::unique_lock<std::mutex> lock(mutex);
        switch(state) {
        case State::paused:
            break;
        case State::running:
            state = State::pausing;
            if (sleepUntilPaused) {
                condition.wait(lock, [this]() -> bool { return this->state != State::pausing; });
            }
            break;
        default:
            throw std::logic_error("Cannot resume: object is not in 'running' state.");
        }
    }

    /**
     * Checks if this object is in a 'running' state.
     *
     * If the control thread requested the simulation to be paused, sends an acknowledgment.
     *
     * @return True if the simulation is in running state.
     */
    bool isRunning() {
        State readState = state;
        if (readState == State::pausing) {
            state = readState = State::paused;
            condition.notify_all();
        }
        return (readState == State::running);
    }
private:
    /** Enumeration of possible states. */
    enum class State {
        running, /**< Simulation is progressing. */
        pausing, /**< Worker thread asked to pause the simulation. */
        paused   /**< Worker thread acknowledged the pause order. */
    };

    /** State of this object. */
    std::atomic<State> state;
    /** Condition variable used by the control thread to wait for acknowledgments from the worker thread. */
    std::condition_variable condition;
    /** Mutex protecting the state variable. */
    std::mutex mutex;
};

class Insight : public LuaVirtualClass {
private:
    using timer = std::chrono::steady_clock;

    /** Shell interpreter callbacks for Insight. */
    class ShellConfig : public ShellInterpreterConfig {
    public:
        /**
         * Make a new Insight shell config.
         * @param insight Object owning this shell config.
         * @param initScripts List of script to execute when starting the shell.
         */
        ShellConfig(Insight& insight, const std::vector<std::string>& initScripts) :
            insight(insight),
            mustResume(false),
            initScripts(initScripts)
        {

        }

        void init(LuaStateView& state) override {
            using Lib = LuaStateView::Lib;
            state.openLib(Lib::base);
            state.openLib(Lib::math);
            state.openLib(Lib::table);

            state.openLib(Lib::package);
            std::string cmd = "package.path = package.path .. ';";
            cmd+= insight.frameworkDir + "/lua/?.lua'";
            state.doString(cmd);

            state.push<Insight*>(&insight);
            state.setGlobal("insight");
            for (const auto& script : initScripts) {
                state.doFile(script);
            }
        }


        void beforeCommand(LuaStateView& state) override {
            mustResume = insight.pauseWorker(true);
        }

        void afterCommand(LuaStateView& state) override {
            if (mustResume && insight.insightState.isPaused()) {
                insight.resumeWorker();
            }
        }
    private:
        /** Insight instance owning this config.*/
        Insight& insight;
        /** Resume the worker thread in afterCommand, if it was paused by beforeCommand.*/
        bool mustResume;
        /** List of scripts to execute when the shell is starting. */
        const std::vector<std::string>& initScripts;
    };

    /** World & physics engine. */
    World world;
    /** List of robots. */
    std::unordered_set<std::shared_ptr<Robot>> robots;
    /** Graphics engine. */
    GraphicEngine graphicEngine;
    /** Shell configuration. */
    ShellConfig shellConfig;
    /** Lua shell (stdin/stdout) interpreter. */
    ShellInterpreter interpreter;
    /** Time between two renders (inverse of the framerate). */
    std::chrono::duration<std::int64_t, std::nano> renderPeriod;
    /** Holds the state of this object, and handles worker thread control (stop & pause). */
    InsightState insightState;
    /** Holds the state of the simulation, and handles worker thread control (can skip simulation and run the GUI only). */
    SimulationState simulationState;
    /** Directory containing the framework. */
    std::string frameworkDir;

    /**
     * Worker thread main loop : computes the simulation.
     *
     * Other threads (Lua interpreter thread) can pause or stop this loop using
     * insightState.
     */
    void workerMainLoop() {
        while (insightState.waitRunningState()) {
            auto start = timer::now();
            if (simulationState.isRunning()) {
                // physics
                world.stepSimulation(std::chrono::duration<double>(renderPeriod).count());
                // AI
                for (auto& robot : robots) {
                    robot->ai->stepSimulation();
                }
            }
            // gui
            graphicEngine.run();

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
     *
     * @param[in] luaInitScripts List of Lua scripts to execute when starting the shell.
     * @param[in] frameworkDir Path to the framework.
     */
    Insight(const std::vector<std::string>& luaInitScripts, const std::string& frameworkDir) :
        graphicEngine(world),
        shellConfig(*this, luaInitScripts),
        interpreter(shellConfig),
        renderPeriod(std::chrono::nanoseconds(1000000000/60)),
        frameworkDir(frameworkDir)
    {

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
     * Pauses the simulation.
     * @param[in] sleepUntilPaused True to wait until the worker thread stopped. False to just send a pause signal.
     */
    void pause(bool sleepUntilPaused) {
        simulationState.pause(sleepUntilPaused);
    }

    /**
     * Resumes the simulation.
     */
    void resume() {
        simulationState.resume();
    }

    /**
     * Starts the different components (Lua interpreter, simulation).
     *
     * This function will return when quit() has been called and all components
     * are stopped.
     */
    void run() {
        insightState.boot();
        simulationState.pause(false);

        std::thread shellThread([this]() { this->interpreter.run(); });
        workerMainLoop();

        shellThread.join();
        insightState.stop();
    }

    /**
     * Pauses the worker thread.
     *
     * This function sends a pause request to the worker thread, and waits
     * until the worker acknowledges the request.
     *
     * @param[in] sleepUntilPaused True to wait until the worker thread stopped. False to just send a pause signal.
     * @return True if this call actually paused the thread (it was not paused before).
     */
    bool pauseWorker(bool sleepUntilPaused) {
        return insightState.pause(sleepUntilPaused);
    }

    /**
     * Resumes the worker thread.
     *
     * This function just sends a resume request to the worker thread.
     */
    void resumeWorker() {
        insightState.resume();
    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override {
        using Method = LuaMethod<Insight>;
        int result = 1;
        if (memberName == "world") {
            state.push<World*>(&world);
        } else if (memberName == "newRobotInfo") {
            state.push<LuaFunction>([](LuaStateView& state) -> int {
                using ConstructionInfo = RobotBody::ConstructionInfo;
                auto newInfo = std::make_shared<ConstructionInfo>(state.get<RobotBody::ConstructionInfo>(1));
                state.push<std::shared_ptr<RobotBody::ConstructionInfo>>(newInfo);
                return 1;
            });
        } else if (memberName == "newRobot") {
            state.push<Method>([](Insight& object, LuaStateView& state) -> int {
                auto bodyInfo = state.get<std::shared_ptr<RobotBody::ConstructionInfo>>(2);
                AIFactory aiFactory = state.get<AIFactory>(3);
                auto newRobot = std::make_shared<Robot>(object.world, bodyInfo, aiFactory);
                state.push<std::shared_ptr<Robot>>(newRobot);
                object.robots.insert(newRobot);
                return 1;
            });
        } else if (memberName == "graphicEngine") {
            state.push<GraphicEngine*>(&graphicEngine);
        } else if (memberName == "quit") {
            state.push<Method>([](Insight& object, LuaStateView& state) -> int {
                object.quit();
                return 0;
            });
        } else if (memberName == "pause") {
            state.push<Method>([](Insight& object, LuaStateView& state) -> int {
                object.pause(false);
                return 0;
            });
        } else if (memberName == "resume") {
            state.push<Method>([](Insight& object, LuaStateView& state) -> int {
                object.resume();
                return 0;
            });
        } else if (memberName == "dir") {
            state.push<LuaNativeString>(frameworkDir.c_str());
        } else {
            result = 0;
        }
        return result;
    }

};

/** Object holding options parsed from the command line. */
struct InsightOptions {
public:
    using variable_map = boost::program_options::variables_map;
    using options_description = boost::program_options::options_description;

    /** Structure holding command line switch literals. */
    struct Switch  {
        /** Prints help message. */
        static constexpr char help[] = "help";
        /** Prints version. */
        static constexpr char version[] = "version";
        /** Execute Lua initialisation script(s). */
        static constexpr char luaInit[] = "luaInit";
        /** Sets the framework base directory. */
        static constexpr char insightDir[] = "insightDir";
        /** Disable automatic execution of init.lua in insightDir. */
        static constexpr char noDefaultInit[] = "noDefaultInit";
    };

    /**
     * Prints an help message describing command line options.
     * @param[out] stream Output stream.
     */
    static void printHelp(std::ostream& stream) {
        stream << description << std::endl;
    }

    /**
     * Creates a new object by parsing command line arguments.
     *
     * @param argc Number of arguments.
     * @param argv Argument values.
     */
    InsightOptions(int argc, char** argv) {
        variable_map variables = parse(argc, argv);
        help = (variables.count(Switch::help) > 0);
        version = (variables.count(Switch::version) > 0);
        insightDir = variables[Switch::insightDir].as<std::string>();
        luaInit = variables[Switch::luaInit].as<std::vector<std::string>>();
        bool noDefaultInit = (variables.count(Switch::noDefaultInit) > 0);
        if (!noDefaultInit) {
            boost::filesystem::path initFile(insightDir + "/init.lua");
            if (boost::filesystem::exists(initFile)) {
                luaInit.push_back(initFile.generic_string());
            }
        }
    }

    /** Help message requested. */
    bool help;
    /** Version requested. */
    bool version;
    /** List of scripts to execute when starting the program. */
    std::vector<std::string> luaInit;
    /** Framework base directory. */
    std::string insightDir;
private:
    /** Object holding the options descriptions. */
    static const options_description description;

    /**
     * Gets the full path of the folder containing this executable.
     *
     * This does NOT return the current working directory.
     *
     * @return The full path to the executable.
     */
    static std::string getBinaryDir() {
        return boost::dll::program_location().parent_path().generic_string();
    }

    /** Initializer of InsightOptions::description. */
    static options_description initDescription() {
        namespace po = boost::program_options;
        options_description result("Command line arguments");
        result.add_options()
            (Switch::help, "prints this help message, and exits.")
            (Switch::insightDir, po::value<std::string>()->default_value(getBinaryDir(),"executable location"), "sets the framework base directory.")
            (Switch::luaInit, po::value<std::vector<std::string>>()->default_value(std::vector<std::string>(),""), "executes a Lua script when starting the program.")
            (Switch::noDefaultInit, "disables automatic execution of init.lua in the framework directory.")
            (Switch::version, "prints version & license info and exits.")
        ;
        return result;
    }

    /**
     * Parses the command line arguments into Boost variable_map format.
     * @param argc Number of arguments.
     * @param argv Argument values.
     * @return A map containing the parsed command line options.
     */
    static variable_map parse(int argc, char **argv) {
        namespace po = boost::program_options;
        po::variables_map result;
        po::store(po::parse_command_line(argc, argv, description), result);
        po::notify(result);
        return result;
    }
};

const boost::program_options::options_description InsightOptions::description(InsightOptions::initDescription());

/**
 * Prints informations about version, license & copyright at program startup.
 */
static void printHeader() {
    std::cout << "Insight version " << INSIGHT_VERSION << "\n";
    std::cout << "Copyright (C) 2017-2019 Vincent Saulue-Laborde\n";
    std::cout << "This is free software; see the source for copying conditions. ";
    std::cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";
    std::cout << std::endl;
}

/**
 * Prints a help message.
 */
static void printHelp() {
    printHeader();
    InsightOptions::printHelp(std::cout);
}

/*
 *
 */
int main(int argc, char** argv) {
    int errCode = EXIT_SUCCESS;
    try {
        const InsightOptions options(argc, argv);
        if (options.help) {
            printHelp();
        } else if (options.version) {
            printHeader();
        } else {
            printHeader();
            Insight insight(options.luaInit, options.insightDir);
            insight.run();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        errCode = EXIT_FAILURE;
    }
    return errCode;
}


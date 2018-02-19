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

class Insight : public LuaVirtualClass {
private:
    using timer = std::chrono::steady_clock;

    World world;
    GraphicEngine graphicEngine;
    ShellInterpreter interpreter;

    std::atomic_bool isEngineRunning;
    std::chrono::duration<std::int64_t, std::nano> renderPeriod;
public:

    Insight() : graphicEngine(world), interpreter(*this, "insight"), isEngineRunning(false), renderPeriod(std::chrono::nanoseconds(1000000000/60)) {

    }

    void runInterpreter() {
        interpreter.run();
    }

    void quit() {
        interpreter.quit();
        isEngineRunning = false;
    }

    void runEngine() {
        isEngineRunning = true;
        while (isEngineRunning && graphicEngine.run()) {
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

static void runInterpreter(Insight &insight) {
    insight.runInterpreter();
}

/*
 *
 */
int main(int argc, char** argv) {
    Insight insight;

    std::thread shellThread(runInterpreter, std::ref(insight));

    insight.runEngine();

    shellThread.join();

    return EXIT_SUCCESS;
}


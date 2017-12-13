/* This file is part of Insight.
 * Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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
#include "PhysicEngine.hpp"
#include "Universe.hpp"
#include "ShellClass.hpp"
#include "ShellInterpreter.hpp"

class Insight : public ShellClass {
private:
    using timer = std::chrono::steady_clock;

    PhysicEngine physicEngine;
    GraphicEngine graphicEngine;
    ShellInterpreter interpreter;

    std::atomic_bool isEngineRunning;
    std::chrono::duration<std::int64_t, std::nano> renderPeriod;
public:

    virtual ShellClass& getField(const std::string& fieldName) override {
        if (fieldName == "physicEngine") {
            return physicEngine;
        } else if (fieldName == "interpreter") {
            return interpreter;
        }
        return ShellClass::getField(fieldName);
    }

    Insight() : graphicEngine(physicEngine), interpreter(*this), isEngineRunning(false), renderPeriod(std::chrono::nanoseconds(1000000000/60)) {
        physicEngine.getUniverse().addObject(new PhysicalObject());
    }

    virtual const std::string& getShellClassName() const override {
        static const std::string className("Insight");
        return className;
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
            physicEngine.integrate();
            graphicEngine.doRender();
            auto ellapsed = timer::now() - start;

            if (ellapsed < renderPeriod) {
                auto sleepTime = renderPeriod - ellapsed;
                std::this_thread::sleep_for(sleepTime);
            }
        }
    }

    void evalMethod(const std::string& methodName) override {
        if (methodName == "quit") {
            quit();
        } else {
            ShellClass::evalMethod(methodName);
        }
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


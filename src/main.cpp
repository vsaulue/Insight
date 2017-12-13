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

#include <cstdlib>

#include "ShellClass.hpp"
#include "ShellInterpreter.hpp"

class Insight : public ShellClass {
private:
    ShellInterpreter interpreter;

public:

    virtual ShellClass& getField(const std::string& fieldName) override {
        if (fieldName == "interpreter") {
            return interpreter;
        }
        return ShellClass::getField(fieldName);
    }

    Insight() : interpreter(*this) {}

    virtual const std::string& getShellClassName() const override {
        static const std::string className("Insight");
        return className;
    }

    void runInterpreter() {
        interpreter.run();
    }

    void quit() {
        interpreter.quit();
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

    runInterpreter(insight);

    return EXIT_SUCCESS;
}


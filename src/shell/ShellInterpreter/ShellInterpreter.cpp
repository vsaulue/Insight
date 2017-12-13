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

#include "ShellInterpreter.hpp"
#include "ShellClass.hpp"
#include "ShellVoid.hpp"
#include "NoSuchFieldShellException.hpp"
#include "NoSuchMethodShellException.hpp"

#include "ShellGrammar.hpp"

#include <iostream>

ShellInterpreter::ShellInterpreter(ShellClass& rootObject) : rootObject(rootObject) {
    running = false;
}

void ShellInterpreter::run() {
    running = true;
    std::string line;
    while (running && std::getline(std::cin, line)) {
        std::shared_ptr<ParserStatement> statement;
        std::string::const_iterator first = line.begin();
        std::string::const_iterator last = line.end();
        bool parsed = parse(first, last, statement);

        if (!parsed || first != last) {
            std::cerr << "Match failed." << std::endl;
        } else {
            try {
                statement->eval(*this).shellPrint(std::cout);
                std::cout << std::endl;
            } catch (const NoSuchFieldShellException &e) {
                std::cerr << e.what() << std::endl;
            } catch (const NoSuchMethodShellException &e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}

void ShellInterpreter::quit() {
    running = false;
}


ShellClass& ShellInterpreter::getRootObject() {
    return rootObject;
}

const std::string& ShellInterpreter::getShellClassName() const {
    static const std::string className("Interpreter");
    return className;
}

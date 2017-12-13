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

#include "ShellVoid.hpp"

ShellVoid::ShellVoid() {

}

const std::string& ShellVoid::getShellClassName() const {
    static const std::string className("Void");
    return className;
}

void ShellVoid::shellPrint(std::ostream& stream) const {
    stream << getShellClassName();
}

ShellClass& ShellVoid::getInstance() {
    static ShellVoid singleton;
    return singleton;
}

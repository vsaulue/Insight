/*
 * This file is part of Insight.
 * Copyright (C) 2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#ifndef LUADEFAULTCLASSNAME_HPP
#define LUADEFAULTCLASSNAME_HPP

#include <string>
#include <typeinfo>

#include "boost/core/demangle.hpp"

/**
 * Provides a default Lua class name for a C++ type.
 *
 * @tparam BoundType C++ type to name into Lua.
 */
template<typename BoundType>
class LuaDefaultClassName {
public:
    /**
     * Gets the Lua class name of BoundType.
     *
     * @return The Lua class name of BoundType.
     */
    static const std::string& luaClassName() {
        static const std::string className(boost::core::demangle(typeid(BoundType).name()));
        return className;
    }
};

#endif /* LUADEFAULTCLASSNAME_HPP */

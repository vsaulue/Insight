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

#include "LuaDefaultBindingClass.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaNativeString.hpp"

int LuaDefaultBindingClass::createCounter = 0;
int LuaDefaultBindingClass::deleteCounter = 0;

LuaDefaultBindingClass::LuaDefaultBindingClass(float value) :
    value(value)
{
    createCounter += 1;
}

LuaDefaultBindingClass::LuaDefaultBindingClass(const LuaDefaultBindingClass& obj) :
    value(obj.value)
{
    createCounter += 1;
}


LuaDefaultBindingClass::LuaDefaultBindingClass(LuaDefaultBindingClass&& obj) :
    value(obj.value)
{
    createCounter += 1;
}

LuaDefaultBindingClass::~LuaDefaultBindingClass() {
    deleteCounter += 1;
}

int LuaDefaultBindingClass::operator()(LuaStateView& state) {
    float arg = state.get<float>(1);
    state.push<float>(-arg);
    return 1;
}

int LuaBinding<LuaDefaultBindingClass>::luaIndexImpl(LuaDefaultBindingClass& object, const std::string& memberName, LuaStateView& state)  {
    using Method = LuaMethod<LuaDefaultBindingClass>;
    int result = 1;
    if (memberName=="value") {
        state.push<float>(object.value);
    } else if (memberName=="setValue") {
        state.push<Method>([](LuaDefaultBindingClass& object, LuaStateView& state) -> int {
            object.value = state.get<float>(2);
            return 0;
        });
    } else if (memberName=="add") {
        // returns (argument + object.value)
        state.push<Method>([](LuaDefaultBindingClass& object, LuaStateView& state) -> int {
            float arg = state.get<float>(2);
            state.push<float>(object.value + arg);
            return 1;
        });
    } else {
        result = 0;
    }
    return result;
}

LuaDefaultBindingClass LuaBinding<LuaDefaultBindingClass>::getFromTable(LuaTable& table) {
    float value = table.get<LuaNativeString, float>("value");
    return LuaDefaultBindingClass(value);
}

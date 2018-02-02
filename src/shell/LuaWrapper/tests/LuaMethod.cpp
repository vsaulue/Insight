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

#include <catch.hpp>
#include <iostream>

#include "LuaState.hpp"
#include "LuaDefaultBinding.hpp"
#include "LuaMethod.hpp"

class MethodClass {
private:
    bool& flag;
public:
    MethodClass(bool& flag) : flag(flag) {

    }

    void setFlag(bool value) {
        flag = value;
    }
};

template<>
class LuaBinding<MethodClass> : public LuaDefaultBinding<MethodClass> {
public:

    static const std::string& luaClassName() {
        static const std::string className("TestClass");
        return className;
    }

    static int luaIndexImpl(MethodClass& object, const std::string& memberName, LuaStateView& state) {
        using Method = LuaMethod<MethodClass>;
        int result = 0;
        if (memberName == "setFlag") {
            state.push<Method>([](MethodClass& o, LuaStateView & s) -> int {
                o.setFlag(s.get<bool>(2)); return 0;
            });
            result = 1;
        }
        return result;
    }
};

TEST_CASE("Non polymorphic type method.") {

    GIVEN("A class TestClass binded in Lua.") {
        LuaState state;
        bool flag = false;

        SECTION("I can call a method on a TestClass object.") {
            state.push<MethodClass>(flag);
            state.setGlobal("object");

            state.doString("object:setFlag(true)");
            REQUIRE(flag==true);
        }

        SECTION("I can call a method on a TestClass** object.") {
            MethodClass object(flag);
            MethodClass *ptr = &object;
            state.push<MethodClass**>(&ptr);
            state.setGlobal("object");

            state.doString("object:setFlag(true)");
            REQUIRE(flag == true);
        }
    }
}
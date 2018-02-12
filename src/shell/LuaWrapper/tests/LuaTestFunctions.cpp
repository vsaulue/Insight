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

#include <functional>

#include "lua/LuaState.hpp"
#include "lua/types/LuaCFunction.hpp"
#include "lua/types/LuaFunction.hpp"

static bool flagA = false;

static int setFlagA(lua_State* state) {
    flagA = true;
    return 0;
}

TEST_CASE("LuaCFunction binding") {
    LuaState state;
    flagA = false;

    SECTION("push<LuaCFunction>") {
        state.push<LuaCFunction>(setFlagA); // stack index 1

        SECTION("Call from Lua") {
            std::string funcName("fooBar");
            state.setGlobal(funcName);
            state.doString(funcName + "()");
            REQUIRE(flagA == true);
        }

        SECTION("get<LuaCFunction>") {
            LuaCFunction fromStack = state.get<LuaCFunction>(1);
            REQUIRE(fromStack.function == &setFlagA);
            REQUIRE(flagA == false);
        }
    }
}

TEST_CASE("LuaFunction binding") {
    LuaState state;

    SECTION("push<LuaFunction>") {
        bool called = false;
        state.push<LuaFunction>([&called](LuaStateView& state) -> int { called = true; return 0;});

        SECTION("get<LuaFunction>") {
            LuaFunction funcFromStack = state.get<LuaFunction>(-1);
            state.pop(1);

            REQUIRE(called == false);
            funcFromStack(state);
            REQUIRE(called == true);
        }

        SECTION("getRef<LuaFunction>") {
            LuaFunction& funcFromStack = state.getRef<LuaFunction>(-1);

            REQUIRE(called == false);
            funcFromStack(state);
            REQUIRE(called == true);
        }

        SECTION("Call from Lua") {
            const std::string luaFuncName("someName");
            state.setGlobal(luaFuncName);

            state.doString(luaFuncName + "()");
            REQUIRE(called == true);
        }
    }
}

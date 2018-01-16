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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <functional>

#include "LuaState.hpp"

static bool flagA = false;

static int setFlagA(lua_State* state) {
    flagA = true;
    return 0;
}

TEST_CASE("int(*)(lua_State*) binding") {

    GIVEN("A C function wrapped in Lua.") {
        flagA = false;
        LuaState state;
        state.push<int(*)(lua_State*)>(setFlagA); // stack index 1

        SECTION("I can push it into Lua.") {
            REQUIRE(flagA == false);
        }

        SECTION("I can execute it from a Lua script.") {
            std::string funcName("fooBar");
            state.setGlobal(funcName);
            state.doString(funcName + "()");
            REQUIRE(flagA == true);
        }

        SECTION("I can read it from the Lua stack.") {
            int (*fromStack)(lua_State*) = state.get<int(*)(lua_State*)>(1);
            REQUIRE(fromStack == &setFlagA);
            REQUIRE(flagA == false);
        }
    }
}

static bool flagB = false;

static int setFlagB(LuaStateView& state) {
    flagB = true;
    return 0;
}

TEST_CASE("int(*)(LuaStateView&) binding") {
    GIVEN("A C++ function wrapped in Lua.") {
        flagB = false;
        LuaState state;
        state.push<int(*)(LuaStateView&)>(setFlagB); // stack index 1

        SECTION("I can push it into Lua.") {
            REQUIRE(flagB == false);
        }

        SECTION("I can execute it from a Lua script.") {
            std::string funcName("fooBar");
            state.setGlobal(funcName);
            state.doString(funcName + "()");
            REQUIRE(flagB == true);
        }

        SECTION("I can read it from the Lua stack.") {
            int (*fromStack)(LuaStateView&) = state.get<int(*)(LuaStateView&)>(1);
            REQUIRE(fromStack == &setFlagB);
            REQUIRE(flagB == false);
        }
    }
}

TEST_CASE("std::function<int(LuaStateView&)> binding") {
    GIVEN("A std::function<LuaStateView&> pushed on the Lua stack.") {
        LuaState state;

        bool called = false;

        auto lambda = [&called](LuaStateView& state) -> int {called = true; return 0; };
        std::function<int(LuaStateView&)> function(lambda);

        state.push<std::function<int(LuaStateView&)>>(function);

        SECTION("I can get a copy from Lua.") {
            std::function<int(LuaStateView&)> funcFromStack = state.get<std::function<int(LuaStateView&)>>(-1);
            state.pop(1);

            REQUIRE(called == false);
            funcFromStack(state);
            REQUIRE(called == true);
        }

        SECTION("I can get a reference from Lua.") {
            std::function<int(LuaStateView&)>& funcFromStack(state.getRef<std::function<int(LuaStateView&)>>(-1));

            REQUIRE(called == false);
            funcFromStack(state);
            REQUIRE(called == true);
        }

        SECTION("I can call it from Lua.") {
            const std::string luaFuncName("someName");
            state.setGlobal(luaFuncName);

            state.doString(luaFuncName+"()");
            REQUIRE(called == true);
        }
    }
}

TEST_CASE("bool binding") {
    GIVEN("A C++ function reading a bool from Lua.") {
        bool readValue = false;
        auto lambda = [&readValue](LuaStateView& state) -> int {
            readValue = state.get<bool>(1);
            return 0;
        };
        std::function<int(LuaStateView&)> function(lambda);

        const std::string funcName("luaFuncName");
        LuaState state;
        state.push<std::function<int(LuaStateView&)>>(function);
        state.setGlobal(funcName);

        SECTION("It can read false.") {
            readValue = true;
            state.doString(funcName+"(false)");
            REQUIRE(readValue == false);
        }

        SECTION("It can read true.") {
            readValue = false;
            state.doString(funcName+"(true)");
            REQUIRE(readValue == true);
        }
    }
}

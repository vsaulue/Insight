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

#include <functional>

#include "catch.hpp"

#include "LuaBaseBindings.hpp"
#include "LuaFunctionBindings.hpp"
#include "LuaState.hpp"

TEST_CASE("bool binding") {
    LuaState state;

    SECTION("Type bool") {
        SECTION("Push & get test") {
            state.push<bool>(true);
            REQUIRE(state.get<bool>(-1) == true);
            state.push<bool>(false);
            REQUIRE(state.get<bool>(-1) == false);
        }

        SECTION("From a C++ function reading booleans from Lua") {
            bool readValue = false;
            const std::string funcName("readBool");

            // Push a Lua function that can read a Lua boolean into a C++ bool.
            auto lambda = [&readValue](LuaStateView& state) -> int {
                readValue = state.get<bool>(1);
                return 0;
            };
            std::function<int(LuaStateView&)> function(lambda);
            state.push<std::function<int(LuaStateView&)>>(function);
            state.setGlobal(funcName);

            SECTION("nil is correctly read as false.") {
                state.doString(funcName+"(nil)");
                REQUIRE(readValue == false);
            }

            SECTION("0 is correctly read as true.") {
                state.doString(funcName+"(0)");
                REQUIRE(readValue == true);
            }
        }
    }
}

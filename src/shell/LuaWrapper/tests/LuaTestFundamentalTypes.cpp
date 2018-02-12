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
#include "LuaTestCommon.hpp"

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/LuaState.hpp"
#include "lua/types/LuaFunction.hpp"

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
            defineReadBool(state, readValue);

            SECTION("nil is correctly read as false.") {
                state.doString("readBool(nil)");
                REQUIRE(readValue == false);
            }

            SECTION("0 is correctly read as true.") {
                state.doString("readBool(0)");
                REQUIRE(readValue == true);
            }
        }
    }
}

TEST_CASE("float & double bindings") {
    LuaState state;

    SECTION("push<double>") {
        const double pushedValue = 1.375;
        state.push<double>(pushedValue);

        SECTION("get<double>") {
            double fromStack = state.get<double>(-1);
            REQUIRE(fromStack == pushedValue);
        }

        SECTION("get<float>") {
            float fromStack = state.get<float>(-1);
            REQUIRE(fromStack == pushedValue);
        }
    }

    SECTION("push<float>") {
        const float pushedValue = -123456.75;
        state.push<float>(pushedValue);

        SECTION("get<float>") {
            float fromStack = state.get<float>(-1);
            REQUIRE(fromStack == pushedValue);
        }

        SECTION("get<double>") {
            double fromStack = state.get<double>(-1);
            REQUIRE(fromStack == pushedValue);
        }
    }

    SECTION("Interaction with native Lua numbers") {
        double readValue = 0;
        state.push<LuaFunction>([&readValue](LuaStateView& state) -> int {
            readValue = state.get<double>(1);
            return 0;
        });
        state.setGlobal("readDouble");

        SECTION("Read from Lua") {
            state.doString("readDouble(4.75)");
            REQUIRE(readValue == 4.75);
        }

        SECTION("Push to Lua") {
            state.push<double>(-3.75);
            state.setGlobal("x");

            state.doString("readDouble(x+4)");
            REQUIRE(readValue == 0.25);
        }
    }
}

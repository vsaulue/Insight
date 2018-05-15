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

#include <memory>

#include <catch.hpp>
#include "LuaTestCommon.hpp"
#include "LuaDefaultBindingClass.hpp"

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/std/shared_ptr.hpp"
#include "lua/LuaState.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "lua/types/LuaTable.hpp"

TEST_CASE("LuaDefaultBinding<T> : shared_ptr<T> binding") {
    SECTION("Init lua") {
        LuaState state;
        using std::shared_ptr;
        using std::make_shared;
        using TestedType = shared_ptr<LuaDefaultBindingClass>;

        SECTION("push<shared_ptr<T>>") {
            const float INIT_VALUE = -0.75f;
            TestedType object = make_shared<LuaDefaultBindingClass>(INIT_VALUE);
            state.push<TestedType>(object);

            SECTION("get<shared_ptr<T>>") {
                TestedType fromStack = state.get<TestedType>(-1);
                REQUIRE(fromStack->value == INIT_VALUE);
            }

            SECTION("getRef<shared_ptr<T>>") {
                TestedType& refFromStack = state.getRef<TestedType>(-1);
                refFromStack = nullptr;

                TestedType fromStack = state.get<TestedType>(-1);
                REQUIRE(fromStack == nullptr);
            }

            SECTION("shared_ptr<T> interactions from Lua.") {
                state.pushValue(-1);
                state.setGlobal("object");

                float readValue;
                defineReadFloat(state, readValue);

                SECTION("Read fields from Lua") {
                    state.doString("readFloat(object.value)");
                    REQUIRE(readValue == INIT_VALUE);
                }

                SECTION("Method call from Lua") {
                    state.doString("object:setValue(2020)");
                    state.doString("readFloat(object.value)");
                    REQUIRE(readValue == 2020);
                }

                SECTION("Store method in variable") {
                    state.push<LuaDefaultBindingClass>(55);
                    state.setGlobal("object2");

                    state.doString("method=object.add");
                    state.doString("readFloat(method(object2,5))");
                    REQUIRE(readValue == 60);
                    state.doString("readFloat(method(object,-3))");
                    REQUIRE(readValue == INIT_VALUE-3);
                }

                SECTION("Lua call") {
                    state.doString("readFloat(object(159))");
                    REQUIRE(readValue == -159);
                }
            }
        }

        SECTION("getFromTable") {
            LuaTable table(state);
            const float TEST_VALUE = -375.0f;
            table.set<LuaNativeString,float>("value", TEST_VALUE);
            TestedType fromTable = state.get<TestedType>(-1);
            REQUIRE(fromTable->value == TEST_VALUE);
        }
    }

    // Default delete test.
    REQUIRE(LuaDefaultBindingClass::createCounter == LuaDefaultBindingClass::deleteCounter);
}

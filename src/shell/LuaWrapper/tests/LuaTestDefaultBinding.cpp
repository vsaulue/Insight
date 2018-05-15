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
#include <iostream>

#include <catch.hpp>
#include "LuaTestCommon.hpp"
#include "LuaDefaultBindingClass.hpp"

#include "lua/LuaState.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/types/LuaFunction.hpp"
#include "lua/types/LuaTable.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "lua/bindings/pointers.hpp"

TEST_CASE("LuaDefaultBinding<T> : T binding") {
    SECTION("Init lua") {
        LuaState state;

        SECTION("push<T>") {
            const float INIT_VALUE = -100.0f;
            state.push<LuaDefaultBindingClass>(INIT_VALUE);

            SECTION("get<T>") {
                LuaDefaultBindingClass fromStack = state.get<LuaDefaultBindingClass>(-1);
                REQUIRE(fromStack.value == INIT_VALUE);
            }

            SECTION("getRef<T>") {
                LuaDefaultBindingClass& refFromStack = state.getRef<LuaDefaultBindingClass>(-1);
                refFromStack.value = -8;

                LuaDefaultBindingClass fromStack = state.get<LuaDefaultBindingClass>(-1);
                REQUIRE(fromStack.value == -8);
            }

            SECTION("T interactions from Lua.") {
                state.pushValue(-1);
                state.setGlobal("object");

                float readValue;
                defineReadFloat(state, readValue);

                SECTION("Read fields from Lua") {
                    state.doString("readFloat(object.value)");
                    REQUIRE(readValue == INIT_VALUE);
                }

                SECTION("Method call from Lua") {
                    state.doString("object:setValue(6363)");
                    state.doString("readFloat(object.value)");
                    REQUIRE(readValue == 6363);
                }

                SECTION("Store method in variable") {
                    state.push<LuaDefaultBindingClass>(-1);
                    state.setGlobal("object2");

                    state.doString("method=object.add");
                    state.doString("readFloat(method(object2,5))");
                    REQUIRE(readValue == 4);
                }

                SECTION("Lua call") {
                    state.doString("readFloat(object(159))");
                    REQUIRE(readValue == -159);
                }
            }
        }

        SECTION("getFromTable") {
            LuaTable table(state);
            const float TEST_VALUE = -852.5f;
            table.set<LuaNativeString,float>("value", TEST_VALUE);
            LuaDefaultBindingClass fromTable = state.get<LuaDefaultBindingClass>(-1);
            REQUIRE(fromTable.value == TEST_VALUE);
        }
    }

    // Default delete test.
    REQUIRE(LuaDefaultBindingClass::createCounter == LuaDefaultBindingClass::deleteCounter);
}

TEST_CASE("LuaDefaultBinding<T> : T** binding") {
    SECTION("Init Lua") {
        LuaState state;

        SECTION("push<T**>") {
            const float INIT_VALUE = -654;
            LuaDefaultBindingClass object(INIT_VALUE);
            LuaDefaultBindingClass* ptr = &object;
            state.push<LuaDefaultBindingClass**>(&ptr);

            SECTION("get<T**>") {
                LuaDefaultBindingClass** fromStack = state.get<LuaDefaultBindingClass**>(-1);
                REQUIRE(fromStack == &ptr);
            }

            SECTION("getRef<T**>") {
                LuaDefaultBindingClass**& refFromStack = state.getRef<LuaDefaultBindingClass**>(-1);
                refFromStack = nullptr;

                LuaDefaultBindingClass** fromStack = state.get<LuaDefaultBindingClass**>(-1);
                REQUIRE (fromStack == nullptr);
            }

            SECTION("T** interactions from Lua.") {
                state.pushValue(-1);
                state.setGlobal("object");

                float readValue;
                defineReadFloat(state, readValue);

                SECTION("Read fields from Lua") {
                    state.doString("readFloat(object.value)");
                    REQUIRE(readValue == INIT_VALUE);
                }

                SECTION("Method call from Lua") {
                    state.doString("object:setValue(3636)");
                    state.doString("readFloat(object.value)");
                    REQUIRE(readValue == 3636);
                }

                SECTION("Store method in variable") {
                    // Checks that the same method can be called on T** & T.
                    state.push<LuaDefaultBindingClass>(91);
                    state.setGlobal("object2");

                    state.doString("method=object.add");
                    state.doString("readFloat(method(object2,5))");
                    REQUIRE(readValue == 96);
                    state.doString("readFloat(method(object,25))");
                    REQUIRE(readValue == INIT_VALUE+25);
                }

                // TODO: recursive lua call function.
                /*SECTION("Lua call") {
                    state.doString("readFloat(object(195))");
                    REQUIRE(readValue == -195);
                }*/
            }
        }
    }

    REQUIRE(LuaDefaultBindingClass::createCounter == LuaDefaultBindingClass::deleteCounter);
}

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

#include "catch.hpp"
#include "LuaDefaultBindingClass.hpp"
#include "LuaTestCommon.hpp"

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/pointers.hpp"
#include "lua/LuaState.hpp"

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

                SECTION("Lua call") {
                    state.doString("readFloat(object(195))");
                    REQUIRE(readValue == -195);
                }
            }
        }
    }

    REQUIRE(LuaDefaultBindingClass::createCounter == LuaDefaultBindingClass::deleteCounter);
}

TEST_CASE("Default pointers binding: bool") {
    LuaState state;

    SECTION("Type bool*") {
        bool boolA = false;

        SECTION("push test") {
            state.push<bool*>(&boolA);

            SECTION("get test") {
                bool* fromStack = state.get<bool*>(-1);
                *fromStack = true;
                REQUIRE(boolA);
            }

            SECTION("getRef test") {
                bool boolB = true;
                bool*& refFromStack = state.getRef<bool*>(-1);

                refFromStack = &boolB;

                bool* ptrFromStack = state.get<bool*>(-1);
                REQUIRE(ptrFromStack == &boolB);
            }
        }
    }

    SECTION("Type bool**") {
        bool boolA = false;
        bool* boolPtr = &boolA;

        SECTION("push test") {
            state.push<bool**>(&boolPtr);

            SECTION("get test") {
                bool** fromStack = state.get<bool**>(-1);
                REQUIRE(**fromStack == false);
                boolA = true;
                REQUIRE(**fromStack == true);
            }

            SECTION("getRef test") {
                bool**& refFromStack = state.getRef<bool**>(-1);
                bool boolB = true;
                bool* boolPtrB = &boolB;
                refFromStack = &boolPtrB;

                bool** ptrFromStack = state.get<bool**>(-1);
                REQUIRE(**ptrFromStack == true);
            }
        }
    }
}

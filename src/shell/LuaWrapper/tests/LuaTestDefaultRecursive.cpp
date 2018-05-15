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
#include "LuaDefaultBindingClass.hpp"
#include "LuaTestCommon.hpp"

#include "lua/bindings/pointers.hpp"
#include "lua/bindings/std/shared_ptr.hpp"
#include "lua/LuaState.hpp"

// Just for the sake of testing that the recursive bindings work as intented: don't do this at home
/// (or at least not in production code).
TEST_CASE("LuaDefaultBinding<T> : std::shared_ptr<T*>*") {
    SECTION("Init Lua") {
        LuaState state;
        using TestedType = std::shared_ptr<LuaDefaultBindingClass*>*;

        SECTION("push<TestedType>") {
            const float INIT_VALUE = -654;
            LuaDefaultBindingClass object(INIT_VALUE);
            std::shared_ptr<LuaDefaultBindingClass*> sptr = std::make_shared<LuaDefaultBindingClass*>(&object);
            state.push<TestedType>(&sptr);

            SECTION("get<TestedType>") {
                TestedType fromStack = state.get<TestedType>(-1);
                REQUIRE(fromStack == &sptr);
            }

            SECTION("getRef<TestedType>") {
                TestedType& refFromStack = state.getRef<TestedType>(-1);
                refFromStack = nullptr;

                TestedType fromStack = state.get<TestedType>(-1);
                REQUIRE (fromStack == nullptr);
            }

            SECTION("TestedType interactions from Lua.") {
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
                    // Checks that the same method can be called on TestedType & T.
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
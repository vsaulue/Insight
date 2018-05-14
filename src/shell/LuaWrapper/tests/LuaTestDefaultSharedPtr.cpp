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
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/bindings/pointers.hpp"
#include "lua/bindings/std/shared_ptr.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/LuaState.hpp"
#include "lua/types/LuaFunction.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaTable.hpp"

class TestSharedPtrClass {
private:
    bool& deleted;
public:
    TestSharedPtrClass(bool& deleteFlag, float value) : deleted(deleteFlag), value(value) {}

    ~TestSharedPtrClass() {
        deleted = true;
    }

    float value;
};

template<>
class LuaBinding<TestSharedPtrClass> : public LuaDefaultBinding<TestSharedPtrClass> {
public:
    static int luaIndexImpl(TestSharedPtrClass& object, const std::string& memberName, LuaStateView& state) {
        using Method = LuaMethod<TestSharedPtrClass>;
        int result = 1;
        if (memberName=="setValue") {
            state.push<Method>([](TestSharedPtrClass& object, LuaStateView& state) -> int {
                object.value = state.get<float>(2);
                return 0;
            });
        } else if (memberName=="value") {
            state.push<float>(object.value);
        } else {
            result = 0;
        }
        return result;
    }

    static TestSharedPtrClass getFromTable(LuaTable& table) {
        bool *flag = table.get<LuaNativeString,bool*>("flag");
        float value = table.get<LuaNativeString,float>("value");
        return TestSharedPtrClass(*flag, value);
    }
};

#include <iostream>

TEST_CASE("std::shared_ptr<TestSharedPtrClass>") {
    using TestedType = std::shared_ptr<TestSharedPtrClass>;
    LuaState state;

    SECTION("state.push<shared_ptr>") {
        float readValue;
        defineReadFloat(state, readValue);

        bool deleted = false;
        const float INIT_VALUE = 3.75f;
        TestedType pushed = std::make_shared<TestSharedPtrClass>(deleted, INIT_VALUE);
        state.push<TestedType>(pushed);

        SECTION("state.get<shared_ptr>") {
            TestedType fromStack = state.get<TestedType>(-1);
            REQUIRE(pushed.get() == fromStack.get());
            fromStack = nullptr;
            pushed = nullptr;
            REQUIRE_FALSE(deleted);
        }

        SECTION("state.getRef<shared_ptr>") {
            TestedType& fromStack = state.getRef<TestedType>(-1);
            REQUIRE(pushed.get() == fromStack.get());
            pushed = nullptr;
            REQUIRE_FALSE(deleted);
            fromStack = nullptr;
            REQUIRE(deleted);
        }

        SECTION("Lua index") {
            state.pushValue(-1);
            state.setGlobal("object");

            SECTION("Lua field") {
                state.doString("readFloat(object.value)");
                REQUIRE(readValue == INIT_VALUE);
            }

            SECTION("Lua method") {
                state.doString("object:setValue(-45)");
                REQUIRE(pushed->value == -45.0f);
            }
        }
    }

    SECTION("getFromTable") {
        bool deleted = false;
        state.push<bool*>(&deleted);
        state.setGlobal("flag");

        float readValue;
        const std::string funcName("testFunc");
        state.push<LuaFunction>([&readValue](LuaStateView& state) -> int {
            TestedType obj = state.get<TestedType>(1);
            readValue = obj->value;
            return 0;
        });
        state.setGlobal(funcName);

        state.doString(funcName+"({flag=flag, value=123})");
        REQUIRE(readValue == 123.0f);
        REQUIRE(deleted);
    }
}

// Just for the sake of testing that the recursive bindings work as intented: don't do this at home
/// (or at least not in production code).
TEST_CASE("std::shared_ptr<TestSharedPtrClass*>*") {
    LuaState state;
    using TestedType = std::shared_ptr<TestSharedPtrClass*>*;

    float readValue;
    defineReadFloat(state, readValue);

    SECTION("state.push<T>") {
        bool deleted = false;
        const float INIT_VALUE = 321.0f;
        TestSharedPtrClass object(deleted, INIT_VALUE);
        std::shared_ptr<TestSharedPtrClass*> shared = std::make_shared<TestSharedPtrClass*>(&object);
        state.push<TestedType>(&shared);

        SECTION("state.get<T>") {
            TestedType fromStack = state.get<TestedType>(-1);
            REQUIRE(fromStack == &shared);
        }

        SECTION("Lua index") {
            state.pushValue(-1);
            state.setGlobal("object");

            SECTION("Lua field") {
                state.doString("readFloat(object.value)");
                REQUIRE(readValue == INIT_VALUE);
            }

            SECTION("Lua method") {
                state.doString("object:setValue(-123)");
                REQUIRE(object.value == -123.0f);
            }
        }
    }
}
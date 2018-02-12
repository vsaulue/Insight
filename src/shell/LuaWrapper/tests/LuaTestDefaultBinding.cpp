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

#include "lua/LuaState.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/bindings/pointers.hpp"

class TestClass {
private:
    bool& flag;
    bool value;
public:
    TestClass(bool& flag) : flag(flag), value(false) {

    }

    void setFlag(bool value) {
        flag = value;
    }

    void setValue(bool value) {
        this->value = value;
    }

    bool& getFlag() {
        return flag;
    }

    bool getValue() {
        return value;
    }
};

template<>
class LuaBinding<TestClass> : public LuaDefaultBinding<TestClass> {
public:
    static int luaIndexImpl(TestClass& object, const std::string& memberName, LuaStateView& state) {
        using Method = LuaMethod<TestClass>;
        int result = 1;
        if (memberName == "setFlag") {
            state.push<Method>([](TestClass& o, LuaStateView & s) -> int {
                o.setFlag(s.get<bool>(2));
                return 0;
            });
        } else if (memberName == "flag") {
            state.push<bool>(object.getFlag());
        } else {
            result = 0;
        }
        return result;
    }
};

TEST_CASE("TestClass default binding") {
    LuaState state;

    SECTION("push<TestClass>") {
        bool flag = false;
        state.push<TestClass>(flag);

        SECTION("get<TestClass>") {
            TestClass fromStack = state.get<TestClass>(-1);
            REQUIRE(&fromStack.getFlag() == &flag);
        }

        SECTION("getRef<TestClass>") {
            TestClass& refFromStack = state.getRef<TestClass>(-1);
            refFromStack.setValue(true);

            TestClass fromStack = state.get<TestClass>(-1);
            REQUIRE(fromStack.getValue() == true);
        }

        SECTION("Method call from Lua") {
            state.setGlobal("object");
            state.doString("object:setFlag(true)");
            REQUIRE(flag==true);
        }

        SECTION("Read fields from Lua") {
            state.setGlobal("object");
            bool readValue = false;
            defineReadBool(state, readValue);

            state.doString("readBool(object.flag)");
            REQUIRE(readValue == flag);
        }
    }
}

TEST_CASE("TestClass** default binding") {
    LuaState state;

    SECTION("push<TestClass**>") {
        bool flag = true;
        TestClass object(flag);
        TestClass* ptr = &object;
        state.push<TestClass**>(&ptr);

        SECTION("get<TestClass**>") {
            TestClass** fromStack = state.get<TestClass**>(-1);
            REQUIRE(fromStack == &ptr);
        }

        SECTION("getRef<TestClass**>") {
            TestClass**& refFromStack = state.getRef<TestClass**>(-1);
            refFromStack = nullptr;

            TestClass** fromStack = state.get<TestClass**>(-1);
            REQUIRE (fromStack == nullptr);
        }

        SECTION("Method call from Lua") {
            state.setGlobal("object");
            state.doString("object:setFlag(false)");
            REQUIRE(flag==false);
        }

        SECTION("Read fields from Lua") {
            state.setGlobal("object");
            bool readValue = false;
            defineReadBool(state, readValue);

            state.doString("readBool(object.flag)");
            REQUIRE(readValue == flag);
        }
    }
}
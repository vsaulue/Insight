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

#include "LuaState.hpp"
#include "LuaBasicBinding.hpp"

class TestClass {
public:

    TestClass(bool a, bool b) : valueA(a), valueB(b) {
    }

    bool valueA;
    bool valueB;
};

template<>
class LuaBinding<TestClass> : public LuaDefaultBinding<TestClass> {
public:

    static const std::string& luaClassName() {
        static const std::string className("TestClass");
        return className;
    }

    static int luaIndexImpl(TestClass& object, const std::string& memberName, LuaStateView& state) {
        int result = 0;
        if (memberName == "a") {
            state.push<bool>(object.valueA);
            result = 1;
        } else if (memberName == "b") {
            state.push<bool>(object.valueB);
            result = 1;
        }
        return result;
    }
};

TEST_CASE("Non polymorphic type member field.") {

    GIVEN("A class TestClass binded in Lua.") {
        LuaState state;

        // function used to read a bool value from Lua.
        bool readValue = false;
        std::function<int(LuaStateView&) > luaReadBool = [&readValue](LuaStateView & state) -> int {
            readValue = state.get<bool>(1);
            return 0;
        };
        state.push<std::function<int(LuaStateView&)>>(luaReadBool);
        state.setGlobal("readBool");

        SECTION("I can read member fields of a TestClass object.") {
            state.push<TestClass>(true, false);
            state.setGlobal("object");

            state.doString("readBool(object.a)");
            REQUIRE(readValue == true);
            state.doString("readBool(object.b)");
            REQUIRE(readValue == false);
        }

        SECTION("I can read member fields of a TestClass** object.") {
            TestClass object(true, false);
            TestClass* ptr = &object;
            state.push<TestClass**>(&ptr);
            state.setGlobal("object");

            state.doString("readBool(object.a)");
            REQUIRE(readValue == true);
            state.doString("readBool(object.b)");
            REQUIRE(readValue == false);
        }
    }
}

TEST_CASE("LuaVirtualClass member field.") {
    class Derived : public LuaVirtualClass {
    public:
        bool valueA;
        bool valueB;

        Derived(bool a, bool b) : valueA(a), valueB(b) {}

        const std::string& luaClassName() const override {
            static const std::string className("Base");
            return className;
        }

        int luaIndex(const std::string& memberName, LuaStateView& state) override {
            int result = 1;
            if (memberName=="a") {
                state.push<bool>(valueA);
            } else if (memberName=="b") {
                state.push<bool>(valueB);
            } else {
                result = 0;
            }
            return result;
        }
    };

    GIVEN("A class hierarchy derived from LuaVirtualClass") {
        LuaState state;

        // function used to read a bool value from Lua.
        bool readValue = false;
        std::function<int(LuaStateView&) > luaReadBool = [&readValue](LuaStateView & state) -> int {
            readValue = state.get<bool>(1);
            return 0;
        };
        state.push<std::function<int(LuaStateView&)>>(luaReadBool);
        state.setGlobal("readBool");

        SECTION("I can read member fields of a Derived object.") {
            state.push<Derived>(true, false);
            state.setGlobal("derived");

            state.doString("readBool(derived.a)");
            REQUIRE(readValue == true);
            state.doString("readBool(derived.b)");
            REQUIRE(readValue == false);
        }

        SECTION("I can read member fields of a Derived** object") {
            Derived derived(false, true);
            Derived* ptr = &derived;
            state.push<Derived**>(&ptr);
            state.setGlobal("derived");

            state.doString("readBool(derived.a)");
            REQUIRE(readValue == false);
            state.doString("readBool(derived.b)");
            REQUIRE(readValue == true);
        }

        SECTION("I can read member fields of a LuaVirtualClass* pointing to a Derived object.") {
            Derived derived(true, false);
            state.push<LuaVirtualClass*>(&derived);
            state.setGlobal("object");

            state.doString("readBool(object.a)");
            REQUIRE(readValue == true);
            state.doString("readBool(object.b)");
            REQUIRE(readValue == false);
        }
    }
}
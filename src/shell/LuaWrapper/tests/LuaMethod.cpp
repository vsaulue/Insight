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
#include <iostream>

#include "LuaState.hpp"
#include "LuaBaseBindings.hpp"
#include "LuaDefaultBinding.hpp"
#include "LuaMethod.hpp"
#include "LuaPointerBindings.hpp"
#include "LuaVirtualClass.hpp"

class MethodClass {
private:
    bool& flag;
public:
    MethodClass(bool& flag) : flag(flag) {

    }

    void setFlag(bool value) {
        flag = value;
    }
};

template<>
class LuaBinding<MethodClass> : public LuaDefaultBinding<MethodClass> {
public:

    static const std::string& luaClassName() {
        static const std::string className("TestClass");
        return className;
    }

    static int luaIndexImpl(MethodClass& object, const std::string& memberName, LuaStateView& state) {
        using Method = LuaMethod<MethodClass>;
        int result = 0;
        if (memberName == "setFlag") {
            state.push<Method>([](MethodClass& o, LuaStateView & s) -> int {
                o.setFlag(s.get<bool>(2)); return 0;
            });
            result = 1;
        }
        return result;
    }
};

TEST_CASE("Non polymorphic type method.") {

    GIVEN("A class TestClass binded in Lua.") {
        LuaState state;
        bool flag = false;

        SECTION("I can call a method on a TestClass object.") {
            state.push<MethodClass>(flag);
            state.setGlobal("object");

            state.doString("object:setFlag(true)");
            REQUIRE(flag==true);
        }

        SECTION("I can call a method on a TestClass** object.") {
            MethodClass object(flag);
            MethodClass *ptr = &object;
            state.push<MethodClass**>(&ptr);
            state.setGlobal("object");

            state.doString("object:setFlag(true)");
            REQUIRE(flag == true);
        }
    }
}

TEST_CASE("Polymorphic type method.") {
    class BaseMethod : public LuaVirtualClass {
    private:
        bool& baseFlag;
    public:
        BaseMethod(bool& flag) : baseFlag(flag) {}

        const std::string& luaClassName() const override {
            static const std::string className("BaseMethod");
            return className;
        }

        int luaIndex(const std::string& memberName, LuaStateView& state) override {
            using Method=LuaMethod<BaseMethod>;
            int result = 0;
            if (memberName=="setBase") {
                state.push<Method>([](BaseMethod& o, LuaStateView& s) -> int { o.setBaseFlag(s.get<bool>(2)); return 0;});
                result = 1;
            }
            return result;
        }

        void setBaseFlag(bool value) {
            baseFlag = value;
        }
    };

    class DerivedMethod : public BaseMethod {
    private:
        bool& derivedFlag;
    public:
        DerivedMethod(bool& baseFlag, bool& derivedFlag) : BaseMethod(baseFlag), derivedFlag(derivedFlag) {}

        const std::string& luaClassName() const override {
            static const std::string className("DerivedMethod");
            return className;
        }

        int luaIndex(const std::string& memberName, LuaStateView& state) override {
            using Method=LuaMethod<DerivedMethod>;
            int result = 0;
            if (memberName=="setDerived") {
                state.push<Method>([](DerivedMethod& o, LuaStateView& s) -> int { o.setDerivedFlag(s.get<bool>(2)); return 0;});
                result = 1;
            } else {
                result = BaseMethod::luaIndex(memberName, state);
            }
            return result;
        }

        void setDerivedFlag(bool value) {
            derivedFlag = value;
        }
    };

    GIVEN("A class hierarchy derived from LuaVirtualClass.") {
        LuaState state;
        bool baseFlag = true;
        bool derivedFlag = false;

        SECTION("I can call all methods on DerivedMethod") {
            state.push<DerivedMethod>(baseFlag, derivedFlag);
            state.setGlobal("object");

            state.doString("object:setDerived(true)");
            REQUIRE(derivedFlag == true);
            state.doString("object:setBase(nil)");
            REQUIRE(baseFlag == false);
        }

        SECTION("I can call a method on BaseMethod") {
            state.push<BaseMethod>(baseFlag);
            state.setGlobal("object");

            state.doString("object:setBase(false)");
            REQUIRE(baseFlag == false);
        }

        SECTION("I can call all methods on LuaVirtualClass* (points to DerivedMethod)") {
            DerivedMethod object(baseFlag, derivedFlag);
            state.push<LuaVirtualClass*>(&object);
            state.setGlobal("object");

            state.doString("object:setDerived(true)");
            REQUIRE(derivedFlag == true);
            state.doString("object:setBase(nil)");
            REQUIRE(baseFlag == false);
        }

        SECTION("I can call all methods on BaseMethod** (points to DerivedMethod)") {
            DerivedMethod object(baseFlag, derivedFlag);
            BaseMethod *ptr = &object;
            state.push<BaseMethod**>(&ptr);
            state.setGlobal("object");

            state.doString("object:setDerived(true)");
            REQUIRE(derivedFlag == true);
            state.doString("object:setBase(nil)");
            REQUIRE(baseFlag == false);
        }

        SECTION("I can store a method from BaseMethod in a Lua variable") {
            state.push<BaseMethod>(baseFlag);
            state.setGlobal("object");

            state.doString("method=object.setBase");

            SECTION("And use it on a LuaVirtualClass** variable (points to DerivedMethod)") {
                DerivedMethod object(baseFlag, derivedFlag);
                LuaVirtualClass *ptr = &object;
                state.push<LuaVirtualClass**>(&ptr);
                state.setGlobal("object");

                state.doString("method(object, false)");
                REQUIRE(baseFlag == false);
            }
        }
    }
}
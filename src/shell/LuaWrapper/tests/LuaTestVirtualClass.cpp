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

#include "LuaBaseBindings.hpp"
#include "LuaBindingVirtualClass.hpp"
#include "LuaMethod.hpp"
#include "LuaPointerBindings.hpp"
#include "LuaState.hpp"

class Base : public LuaVirtualClass {
public:
    bool baseValue;

    Base(bool value) : baseValue(value) {

    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override {
        int result = 1;
        if (memberName=="baseValue") {
            state.push<bool>(baseValue);
        } else if (memberName=="setBase") {
            state.push<LuaMethod<Base>>([](Base& o, LuaStateView& s) -> int { o.baseValue = s.get<bool>(2); return 0; });
        } else {
            result = 0;
        }
        return result;
    }
};

class Derived1 : public Base {
public:
    bool derivedValue;

    Derived1(bool bVal, bool dVal) : Base(bVal), derivedValue(dVal) {

    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override {
        int result = 1;
        if (memberName=="derivedValue") {
            state.push<bool>(derivedValue);
        } else if (memberName=="setDerived") {
            state.push<LuaMethod<Derived1>>([](Derived1& o, LuaStateView& s) -> int { o.derivedValue = s.get<bool>(2); return 0; });
        } else {
            result = Base::luaIndex(memberName, state);
        }
        return result;
    }
};

class Derived2 : public Base {
public:
    Derived2(bool value) : Base(value) {

    }
};

TEST_CASE("LuaVirtualClass (& derived types) binding") {
    LuaState state;

    SECTION("push<Derived1>") {
        state.push<Derived1>(true, false);

        SECTION("get<Derived1>") {
            Derived1 fromStack = state.get<Derived1>(-1);
            REQUIRE(fromStack.derivedValue == false);
        }

        SECTION("getRef<Derived1>") {
            Derived1& fromStack = state.getRef<Derived1>(-1);
            REQUIRE(fromStack.derivedValue == false);
        }

        SECTION("getRef<Base> (upcast)") {
            Base& baseFromStack = state.getRef<Base>(-1);
            Derived1& derivedFromStack = state.getRef<Derived1>(-1);
            REQUIRE(&baseFromStack == &derivedFromStack);
        }

        SECTION("getRef<LuaVirtualClass> (upcast)") {
            LuaVirtualClass& baseFromStack = state.getRef<LuaVirtualClass>(-1);
            Derived1& derivedFromStack = state.getRef<Derived1>(-1);
            REQUIRE(&baseFromStack == &derivedFromStack);
        }

        SECTION("getRef<Derived2> (invalid cast, must throw)") {
            bool exception = false;
            try {
                state.getRef<Derived2>(-1);
            } catch (const LuaException& e) {
                exception = true;
            }
            REQUIRE(exception);
        }

        SECTION("Read fields from Lua.") {
            Derived1& ref = state.getRef<Derived1>(-1);
            state.setGlobal("object");

            bool readValue = false;
            defineReadBool(state, readValue);

            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == true);

            ref.baseValue = false;
            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == false);
        }

        SECTION("Call methods from Lua") {
            Derived1& ref = state.getRef<Derived1>(-1);
            state.setGlobal("object");

            state.doString("object:setBase(false)");
            REQUIRE(ref.baseValue == false);
            state.doString("object:setDerived(1)");
            REQUIRE(ref.derivedValue == true);
        }
    }

    SECTION("push<Base>") {
        state.push<Base>(true);

        SECTION("getRef<Derived1> (invalid downcast, must throw)") {
            bool exception = false;
            try {
                state.getRef<Derived1>(-1);
            } catch (const LuaException& e) {
                exception = true;
            }
            REQUIRE(exception);
        }

        SECTION("Read fields from Lua") {
            Base& ref = state.getRef<Base>(-1);
            state.setGlobal("object");

            bool readValue = false;
            defineReadBool(state, readValue);

            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == true);
        }

        SECTION("Call methods from Lua") {
            Base& ref = state.getRef<Base>(-1);
            state.setGlobal("object");

            state.doString("object:setBase(false)");
            REQUIRE(ref.baseValue == false);
        }
    }
}

TEST_CASE("LuaVirtualClass (& derived types) pointers binding") {
    LuaState state;

    SECTION("push<Base*> (points to Derived2)") {
        Derived2 derived(false);
        state.push<Base*>(&derived);

        SECTION("get<Base*>") {
            Base* fromStack = state.get<Base*>(-1);
            REQUIRE(fromStack == &derived);
        }

        SECTION("getRef<Base*>") {
            Base*& refFromStack = state.getRef<Base*>(-1);
            Base newBase(false);
            refFromStack = &newBase;

            Base* ptrFromStack = state.get<Base*>(-1);
            REQUIRE(ptrFromStack == &newBase);
        }

        SECTION("get<LuaVirtualClass*> (upcast)") {
            LuaVirtualClass* fromStack = state.get<LuaVirtualClass*>(-1);
            REQUIRE(fromStack == &derived);
        }

        SECTION("get<Derived2> (downcast)") {
            Derived2* fromStack = state.get<Derived2*>(-1);
            REQUIRE(fromStack == &derived);
        }

        SECTION("get<Derived1> (invalid cast, must throw)") {
            bool error = false;
            try {
                state.get<Derived1*>(-1);
            } catch (const LuaException& e) {
                error = true;
            }
            REQUIRE(error);
        }
    }

    SECTION("push<Derived2*>") {
        Derived2 derived(false);
        state.push<Derived2*>(&derived);

        SECTION("get<Base*> (upcast)") {
            Base* fromStack = state.get<Base*>(-1);
            REQUIRE(fromStack == &derived);
        }

        SECTION("get<LuaVirtualClass*> (upcast)") {
            LuaVirtualClass* fromStack = state.get<LuaVirtualClass*>(-1);
            REQUIRE(fromStack == &derived);
        }

        SECTION("get<Derived1*> (invalid cast)") {
            bool error = false;
            try {
                state.get<Derived1*>(-1);
            } catch (const LuaException& e) {
                error = true;
            }
            REQUIRE(error);
        }
    }

    SECTION("push<LuaVirtualClass*> (points to Derived1)") {
        Derived1 derived(false, false);
        state.push<LuaVirtualClass*>(&derived);
        state.setGlobal("object");

        SECTION("Read fields from Lua") {
            bool readValue = false;
            defineReadBool(state, readValue);

            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == false);
            state.doString("readBool(object.derivedValue)");
            REQUIRE(readValue == false);

            derived.baseValue = true;
            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == true);
        }

        SECTION("Call methods from Lua") {
            state.doString("object:setDerived(true)");
            REQUIRE(derived.derivedValue == true);
            state.doString("object:setBase(nil)");
            REQUIRE(derived.baseValue == false);
        }

        SECTION("Store methods in Lua variables") {
            state.doString("method=object.setBase");

            SECTION("Call stored method on another type") {
                state.push<Base>(false);
                Base& base = state.getRef<Base>(-1);
                state.setGlobal("base");

                state.doString("method(base, true)");
                REQUIRE(base.baseValue == true);
            }
        }
    }
}

TEST_CASE("LuaVirtualClass** (& derived types) bindings") {
    LuaState state;

    SECTION("push<LuaVirtualClass**> (points to Derived1)") {
        Derived1 derived(false, true);
        LuaVirtualClass* ptr = &derived;
        state.push<LuaVirtualClass**>(&ptr);
        state.setGlobal("object");

        SECTION("Read fields from Lua") {
            bool readValue = true;
            defineReadBool(state, readValue);

            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == false);
            state.doString("readBool(object.derivedValue)");
            REQUIRE(readValue == true);
        }
    }

    SECTION("push<Base**> (points to Derived1)") {
        Derived1 derived(true, false);
        Base* ptr = &derived;
        state.push<Base**>(&ptr);
        state.setGlobal("object");

        SECTION("Call methods from Lua") {
            state.doString("object:setDerived(true)");
            REQUIRE(derived.derivedValue == true);
            state.doString("object:setBase(nil)");
            REQUIRE(derived.baseValue == false);
        }
    }
}
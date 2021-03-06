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
#include <memory>

#include "catch.hpp"
#include "LuaTestCommon.hpp"

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "lua/bindings/luaVirtualClass/shared_ptr.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaTable.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "lua/LuaState.hpp"

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

    static std::unique_ptr<Base> luaGetFromTable(LuaTable& table);
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

    static Derived2 luaGetFromTable(LuaTable& table) {
        bool value = table.get<LuaNativeString,bool>("baseValue");
        return Derived2(value);
    }
};

std::unique_ptr<Base> Base::luaGetFromTable(LuaTable& table) {
    std::string type(table.get<LuaNativeString,LuaNativeString>("type"));
    bool baseValue = table.get<LuaNativeString,bool>("baseValue");
    std::unique_ptr<Base> result = nullptr;
    if (type=="Derived1") {
        bool derivedValue = table.get<LuaNativeString,bool>("derivedValue");
        result = std::make_unique<Derived1>(baseValue, derivedValue);
    } else if (type=="Derived2") {
        result = std::make_unique<Derived2>(baseValue);
    } else {
        std::string msg = std::string("Invalid 'type' field in Base table constructor: ") + type;
        throw LuaException(msg.c_str());
    }
    return result;
}

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

    SECTION("Table constructors") {
        LuaTable table(state);
        const bool BASE_VALUE = true;
        table.set<LuaNativeString,bool>("baseValue", BASE_VALUE);

        SECTION("Base table constructor") {
            SECTION("table {type=Derived1, ...}") {
                const bool DERIVED_VALUE = false;
                table.set<LuaNativeString,LuaNativeString>("type", "Derived1");
                table.set<LuaNativeString,bool>("derivedValue", DERIVED_VALUE);
                Base base = state.get<Base>(table.getStackIndex());
                REQUIRE(base.baseValue == BASE_VALUE);
            }

            SECTION("table {type=Derived2, ...}") {
                table.set<LuaNativeString,LuaNativeString>("type", "Derived2");
                Base base = state.get<Base>(table.getStackIndex());
                REQUIRE(base.baseValue == BASE_VALUE);
            }
        }

        SECTION("Derived2 table constructor") {
            Derived2 derived2 = state.get<Derived2>(table.getStackIndex());
            REQUIRE(derived2.baseValue == BASE_VALUE);
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

template<typename T>
using shared_ptr = std::shared_ptr<T>;

TEST_CASE("std::shared_ptr<LuaVirtualClass> (derived shared_ptrs) bindings.") {
    LuaState state;

    SECTION("push<shared_ptr<Base>> (points to Derived2)") {
        std::shared_ptr<Derived2> derived = std::make_shared<Derived2>(false);
        state.push<shared_ptr<Base>>(derived);

        SECTION("get<shared_ptr<Base>>") {
            shared_ptr<Base> fromStack = state.get<shared_ptr<Base>>(-1);
            REQUIRE(fromStack == derived);
        }

        SECTION("getRef<shared_ptr<Base>>") {
            shared_ptr<Base>& refFromStack = state.getRef<shared_ptr<Base>>(-1);
            shared_ptr<Base> newBase = std::make_shared<Base>(false);
            refFromStack = newBase;

            shared_ptr<Base> ptrFromStack = state.get<shared_ptr<Base>>(-1);
            REQUIRE(ptrFromStack == newBase);
        }

        SECTION("get<shared_ptr<LuaVirtualClass>> (upcast)") {
            shared_ptr<LuaVirtualClass> fromStack = state.get<shared_ptr<LuaVirtualClass>>(-1);
            REQUIRE(fromStack == derived);
        }

        SECTION("get<shared_ptr<Derived2>> (downcast)") {
            shared_ptr<Derived2> fromStack = state.get<shared_ptr<Derived2>>(-1);
            REQUIRE(fromStack == derived);
        }

        SECTION("get<shared_ptr<Derived1>> (invalid cast, must throw)") {
            bool error = false;
            try {
                state.get<shared_ptr<Derived1>>(-1);
            } catch (const LuaException& e) {
                error = true;
            }
            REQUIRE(error);
        }
    }

    SECTION("push<shared_ptr<Derived2>>") {
        shared_ptr<Derived2> derived = std::make_shared<Derived2>(false);
        state.push<shared_ptr<Derived2>>(derived);

        SECTION("get<shared_ptr<Base>> (upcast)") {
            shared_ptr<Base> fromStack = state.get<shared_ptr<Base>>(-1);
            REQUIRE(fromStack == derived);
        }

        SECTION("get<shared_ptr<LuaVirtualClass>> (upcast)") {
            shared_ptr<LuaVirtualClass> fromStack = state.get<shared_ptr<LuaVirtualClass>>(-1);
            REQUIRE(fromStack == derived);
        }

        SECTION("get<shared_ptr<Derived1>> (invalid cast)") {
            bool error = false;
            try {
                state.get<shared_ptr<Derived1>>(-1);
            } catch (const LuaException& e) {
                error = true;
            }
            REQUIRE(error);
        }
    }

    SECTION("push<shared_ptr<Base>> (points to Derived1)") {
        shared_ptr<Derived1> derived = std::make_shared<Derived1>(false, false);
        state.push<shared_ptr<Base>>(derived);
        state.setGlobal("object");

        SECTION("Read fields from Lua") {
            bool readValue = false;
            defineReadBool(state, readValue);

            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == false);
            state.doString("readBool(object.derivedValue)");
            REQUIRE(readValue == false);

            derived->baseValue = true;
            state.doString("readBool(object.baseValue)");
            REQUIRE(readValue == true);
        }

        SECTION("Call methods from Lua") {
            state.doString("object:setDerived(true)");
            REQUIRE(derived->derivedValue == true);
            state.doString("object:setBase(nil)");
            REQUIRE(derived->baseValue == false);
        }

        SECTION("Store methods in Lua variables") {
            state.doString("method=object.setBase");

            SECTION("Call stored method on another type") {
                state.push<shared_ptr<Base>>(std::make_shared<Base>(false));
                shared_ptr<Base>& base = state.getRef<shared_ptr<Base>>(-1);
                state.setGlobal("base");

                state.doString("method(base, true)");
                REQUIRE(base->baseValue == true);
            }
        }
    }

    SECTION("Table constructors") {
        LuaTable table(state);
        const bool BASE_VALUE = true;
        table.set<LuaNativeString,bool>("baseValue", BASE_VALUE);

        SECTION("shared_ptr<Base> table constructor") {
            SECTION("table {type=Derived1, ...}") {
                const bool DERIVED_VALUE = false;
                table.set<LuaNativeString,LuaNativeString>("type", "Derived1");
                table.set<LuaNativeString,bool>("derivedValue", DERIVED_VALUE);
                std::shared_ptr<Base> base = state.get<std::shared_ptr<Base>>(table.getStackIndex());
                REQUIRE(base->baseValue == BASE_VALUE);
                Derived1* derived1 = dynamic_cast<Derived1*>(base.get());
                REQUIRE(derived1 != nullptr);
                REQUIRE(derived1->derivedValue == DERIVED_VALUE);

            }

            SECTION("table {type=Derived2, ...}") {
                table.set<LuaNativeString,LuaNativeString>("type", "Derived2");
                std::shared_ptr<Base> base = state.get<std::shared_ptr<Base>>(table.getStackIndex());
                REQUIRE(base->baseValue == BASE_VALUE);
            }
        }

        SECTION("shared_ptr<Derived2> table constructor") {
            std::shared_ptr<Derived2> derived2 = state.get<std::shared_ptr<Derived2>>(table.getStackIndex());
            REQUIRE(derived2->baseValue == BASE_VALUE);
        }
    }
}

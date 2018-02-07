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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <functional>

#include "LuaState.hpp"

static bool flagA = false;

static int setFlagA(lua_State* state) {
    flagA = true;
    return 0;
}

TEST_CASE("int(*)(lua_State*) binding") {

    GIVEN("A C function wrapped in Lua.") {
        flagA = false;
        LuaState state;
        state.push<int(*)(lua_State*)>(setFlagA); // stack index 1

        SECTION("I can push it into Lua.") {
            REQUIRE(flagA == false);
        }

        SECTION("I can execute it from a Lua script.") {
            std::string funcName("fooBar");
            state.setGlobal(funcName);
            state.doString(funcName + "()");
            REQUIRE(flagA == true);
        }

        SECTION("I can read it from the Lua stack.") {
            int (*fromStack)(lua_State*) = state.get<int(*)(lua_State*)>(1);
            REQUIRE(fromStack == &setFlagA);
            REQUIRE(flagA == false);
        }
    }
}

static bool flagB = false;

static int setFlagB(LuaStateView& state) {
    flagB = true;
    return 0;
}

TEST_CASE("int(*)(LuaStateView&) binding") {
    GIVEN("A C++ function wrapped in Lua.") {
        flagB = false;
        LuaState state;
        state.push<int(*)(LuaStateView&)>(setFlagB); // stack index 1

        SECTION("I can push it into Lua.") {
            REQUIRE(flagB == false);
        }

        SECTION("I can execute it from a Lua script.") {
            std::string funcName("fooBar");
            state.setGlobal(funcName);
            state.doString(funcName + "()");
            REQUIRE(flagB == true);
        }

        SECTION("I can read it from the Lua stack.") {
            int (*fromStack)(LuaStateView&) = state.get<int(*)(LuaStateView&)>(1);
            REQUIRE(fromStack == &setFlagB);
            REQUIRE(flagB == false);
        }
    }
}

TEST_CASE("std::function<int(LuaStateView&)> binding") {
    GIVEN("A std::function<LuaStateView&> pushed on the Lua stack.") {
        LuaState state;

        bool called = false;

        auto lambda = [&called](LuaStateView& state) -> int {called = true; return 0; };
        std::function<int(LuaStateView&)> function(lambda);

        state.push<std::function<int(LuaStateView&)>>(function);

        SECTION("I can get a copy from Lua.") {
            std::function<int(LuaStateView&)> funcFromStack = state.get<std::function<int(LuaStateView&)>>(-1);
            state.pop(1);

            REQUIRE(called == false);
            funcFromStack(state);
            REQUIRE(called == true);
        }

        SECTION("I can get a reference from Lua.") {
            std::function<int(LuaStateView&)>& funcFromStack = state.getRef<std::function<int(LuaStateView&)>>(-1);

            REQUIRE(called == false);
            funcFromStack(state);
            REQUIRE(called == true);
        }

        SECTION("I can call it from Lua.") {
            const std::string luaFuncName("someName");
            state.setGlobal(luaFuncName);

            state.doString(luaFuncName+"()");
            REQUIRE(called == true);
        }
    }
}

#include "LuaVirtualClass.hpp"
#include "LuaException.hpp"

class Derived1 : public LuaVirtualClass {
public:

    const std::string& luaClassName() const override {
        static const std::string className("Derived1");
        return className;
    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override {
        return 0;
    }

};

class Derived1A : public Derived1 {
public:
    const std::string& luaClassName() const override {
        static const std::string className("Derived1A");
        return className;
    }
};

class Derived1B : public Derived1 {
public:
    const std::string& luaClassName() const override {
        static const std::string className("Derived1B");
        return className;
    }
};

TEST_CASE("LuaVirtualClass binding") {

    GIVEN("A class hierarchy derived from LuaVirtualClass") {
        LuaState state;

        SECTION("I can push an object in the Lua stack.") {
            state.push<Derived1A>();

            SECTION("I can get it from Lua") {
                const std::string& className = state.get<Derived1A>(-1).luaClassName();
                REQUIRE(className == "Derived1A");
            }

            SECTION("I can getRef it from Lua") {
                Derived1A& ref = state.getRef<Derived1A>(-1);
                const std::string& className = ref.luaClassName();
                REQUIRE(className == "Derived1A");
            }

            SECTION("I can upcast it.") {
                Derived1& derived = state.getRef<Derived1>(-1);
                const std::string& derivedClassName = derived.luaClassName();
                REQUIRE(derivedClassName == "Derived1A");

                LuaVirtualClass& luaClass = state.getRef<LuaVirtualClass>(-1);
                const std::string& luaClassName = luaClass.luaClassName();
                REQUIRE(luaClassName == "Derived1A");
            }

            SECTION("I have an exception in case of invalid 'lateral' cast.") {
                bool exception = false;
                try {
                    state.getRef<Derived1B>(-1);
                } catch (const LuaException& e) {
                    exception = true;
                }
                REQUIRE(exception);
            }
        }

        SECTION("I can push an object in the Lua stack.") {
            state.push<Derived1>();

            SECTION("I have an exception in case of invalid downcast.") {
                bool exception = false;
                try {
                    state.getRef<Derived1A>(-1);
                } catch (const LuaException& e) {
                    exception = true;
                }
                REQUIRE(exception);
            }
        }
    }
}

TEST_CASE("Pointers binding.") {
    GIVEN("A class hierarchy derived from LuaVirtualClass.") {
        LuaState state;

        SECTION("I can push a pointer to an instance of LuaVirtualClass into the stack.") {
            Derived1A derived;
            state.push<Derived1*>(&derived);

            SECTION("I can get it from the stack.") {
                Derived1* fromStack = state.get<Derived1*>(-1);
                REQUIRE(fromStack == static_cast<Derived1*>(&derived));
            }

            SECTION("I can getRef it from the stack.") {
                Derived1*& fromStack = state.getRef<Derived1*>(-1);
                REQUIRE(fromStack == static_cast<Derived1*>(&derived));
            }

            SECTION("I can downcast it from the stack (if type is appropriate).") {
                Derived1A* fromStack = state.get<Derived1A*>(-1);
                REQUIRE(fromStack == &derived);
            }

            SECTION("I have an exception in case of invalid downcast.") {
                bool error = false;
                try {
                    state.get<Derived1B*>(-1);
                } catch (const LuaException& e) {
                    error = true;
                }
                REQUIRE(error);
            }
        }

        SECTION("I can push a pointer to an instance of LuaVirtualClass into the stack.") {
            Derived1B derived;
            state.push<Derived1B*>(&derived);

            SECTION("I can upcast it from the stack.") {
                Derived1* fromStack = state.get<Derived1*>(-1);
                REQUIRE(fromStack == static_cast<Derived1*>(&derived));

                LuaVirtualClass* fromStack2 = state.get<LuaVirtualClass*>(-1);
                REQUIRE(fromStack2 == static_cast<LuaVirtualClass*>(&derived));
            }

            SECTION("I have an exception in case of invalid 'lateral' cast.") {
                bool error = false;
                try {
                    state.get<Derived1A*>(-1);
                } catch (const LuaException& e) {
                    error = true;
                }
                REQUIRE(error);
            }
        }
    }
}

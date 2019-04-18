# Insight - LuaWrapper library

This library is a C++ wrapper of the [Lua C API](https://www.lua.org/pil/24.html). The main features of this library are:
- a C++ friendly interface (RAII, Exceptions, std::function and lambdas)
- helper class to define powerful bindings between Lua and C++ objects:
  - recursive bindings: possibility to manipulate `MyClass`, `MyClass*`, `MyClass*****`, `shared_ptr<MyClass>` objects in Lua just by binding `MyClass`
  - multiple bindings methods: non-intrusive for 3rd party code, or by deriving from a specific class. The later handles polymorphism between Lua and C++: a `Derived*` object in the Lua stack can be read as a `Base*` object, a `Base*` object can even be read as a `Derived*` object (the library will `dynamic_cast` as needed, and throw an exception if the cast is invalid).
  - on the fly binding registering: C++ class will be registered into the Lua interpreter the first time they are used.

# Usage

A decent understanding of the [Lua C API](https://www.lua.org/pil/24.html) is needed to use this library. Especially about the notions of userdata & metatables, Lua stacks & functions/methods.

## Hello world: create a Lua interpreter & run commands

Lua C states can be manipulated through the [LuaStateView](include/lua/LuaStateView.hpp) class. [LuaState](include/lua/LuaState.hpp) is a derived class which creates and acquires ownership of a C state: it can be used to create an interpreter.

```
#include "lua/LuaState.hpp"

int main(void) {
    LuaState state;
    state.openLib(LuaStateView::Lib::base);
    state.doString("print('Hello world!')");
    return 0;
}
```

## Stack manipulation

### Pushing C++ objects on the Lua stack

New C++ objects can be constructed on a Lua stack with `LuaStateView::push<SomeType>(/* SomeType constructor args */)`. This method will attach (create if needed) a metatable to enable the manipulation of the object from Lua.

```
#include <string>

#include "lua/LuaState.hpp"
#include "lua/bindings/std/string.hpp"

int main(void) {
    LuaState state;
    state.openLib(LuaStateView::Lib::base);
    state.push<std::string>("Hello world !");
    state.setGlobal("myGlobal");
    state.doString("print(myGlobal)");
    return 0;
}
```

### Reading objects in the Lua stack

There are multiple ways of accessing values of an object in the Lua stack:
- `LuaStateView::get<SomeType>(stackIndex);` gets a copy of the object of type `SomeType` (or a compatible type) at the specified index. It can also be used to constructs C++ objects from Lua tables.
- `LuaStateView::getRef<SomeType>(stackIndex);` gets a reference to the value stored in Lua. This method can't invoke a table constructor, or convert native Lua types.
- `LuaStateView::dereferenceGet<SomeType>(stackIndex);` recursively dereferences (smart) pointers to return a reference to the underlying object. This is how the same Lua method can work indifferently on `SomeType`, `shared_ptr<SomeType>` C++ objects for example.

```
#include <string>

#include "lua/LuaState.hpp"
#include "lua/bindings/std/string.hpp"

int main(void) {
    LuaState state;
    state.openLib(LuaStateView::Lib::base);
    // Creates a std::string in the string. Make it available from this Lua stack and as a global.
    state.push<std::string>("Hello world !");
    state.pushValue(-1);
    state.setGlobal(myGlobal);

    auto copy = state.get<std::string>(-1);
    copy += " xyz";
    state.doString("print(myGlobal)"); // prints "Hello world !"

    auto& ref = state.getref<std::string>(-1);
    ref += " foo";
    state.doString("print(myGlobal)"); // prints "Hello world ! foo"

    auto& ref2 = state.dereferenceGet<std::string>(-1);
    ref2 += "bar";
    state.doString("print(myGlobal)"); // prints "Hello world ! foobar"
    return 0;
}
```

## Using C++ functions from Lua

The library contains the [LuaFunction](include/lua/types/LuaFunction.hpp) class to use C++ code from Lua. This class wraps an object of type `std::function<int(LuaStateView&)>`, and can be constructed with the same arguments. The parameter will be the state invoking this function. The returned `int` is the number of return values (returns values should be left on the top of the Lua stack).

```
#include "lua/LuaState.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaFunction.hpp"

int main(void) {
    LuaState state;
    state.openLib(LuaStateView::Lib::base);

    float incVal = 42;
    state.push<LuaFunction>([&incVal](LuaStateView& state) -> int {
        float arg = state.get<float>(1);
        state.push<float>(arg + incVal);
        return 1;
    ];
    state.setGlobal("myFunc");

    state.doString("myFunc(11)"); // prints 43
    incVal = 4;
    state.doString("myFunc(5)"); // prints 9

    return 0;
}
```

A similar templated class is provided to implement Lua methods in C++: [LuaMethod](include/lua/types/LuaMethod.hpp).

## Lua table wrapper

The [LuaTable](include/lua/types/LuaTable.hpp) is provided in order to read/modify a Lua table.

## Custom class bindings

In the general case, a custom binding for the C++ type `SomeType` (including pointer, reference types) is defined by specializing [LuaBinding](include/lua/LuaBinding.hpp) for `SomeType`. This specialization should provide:
- a `push` static function to create `SomeType` objects on the Lua stack.
- `get`, `getRef`, `dereferenceGet` static functions to read `SomeType` objects from the stack.

This library provides 2 utilities to simply write such a binding:
- inheriting `LuaBinding<SomeType>` from [LuaDefaultBinding](include/lua/bindings/LuaDefaultBinding.hpp):
  - non intrusive (the bound C++ type does not require any modification).
  - no polymorphism support (if a `Derived` object is pushed, it can't be accessed via `getRef<Base>`).
  - unable to access private/protected member of the object.
- By inheriting the bound C++ type from [LuaVirtualClass](include/lua/types/LuaVirtualClass.hpp):
  -  requires to modify the bound type, and add virtual methods.
  -  polymorphism support (push a `Derived` object, access it later via `getRef<Base>`).
  -  can access private/protected members.

Both utilities come will be able to provide bindings for `SomeType*` and `std::shared_ptr<SomeType>`.

### LuaDefaultBinding

Using this simple test class:

```
class Counter {
private:
	int value;
public:
	Counter(int initVal) : value(initVal) {}

    void inc() { value++; }
    void setValue(int newVal) { value = newVal; }
    int getValue() const { return value; }
};
```

A simple binding can be defined as:
```
#include "lua/bindings/LuaDefaultBinding.hpp"

template<>
class LuaBinding<Counter> : public LuaDefaultBinding<Counter> {

}
```

This is enough to push/get objects of type `Counter` on/from the Lua stack. By including the headers [lua/bindings/pointers.hpp](include/lua/bindings/pointers.hpp) or [lua/bindings/std:shared_ptr.hpp](include/lua/bindings/std/shared_ptr.hpp), it will also be possible to push/get `SomeType*`, `SomeType***` or `std::shared_ptr<SomeType>` objects.

However this object will just be a black box from the Lua perspective: no members, no nice string representation, no table constructor... Let's define a binding with all of these, by adding a few public static functions to `LuaBinding<Counter>`:
- `static Counter getFromTable(LuaTable&)` enables objects of type `Counter` to be constructed from a Lua table. `LuaStateView::get<Counter>(...)` will do it if the item of the stack is a Lua table.
- `static int luaIndexImpl(Counter&, const std::string&, LuaStateView&)` is the implementation of the Lua index metamthod for this type. The return value is the number of items returned by the Lua function on the stack. the arguments are:
  - the object accessed from Lua
  - the name of the member accessed from Lua
  - The Lua state calling this function
- `static std::string luaToStringImpl(Counter&)` is the implementation of the Lua toString metamethod. It enables Lua to print the logical value of the object.

```
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/bindings/FundamentalTypes.hpp"

template<>
class LuaBinding<Counter> : public LuaDefaultBinding<Counter> {
public:
    static Counter getFromTable(LuaTable& table) {
        int val = table.get<LuaNativeString, float>("value");
        return Counter(val);
    }

    static int luaIndexImpl(Counter& object, const std::string& memberName, LuaStateView& state) {
        using Method = LuaMethod<Counter>;
        int result = 1;
        if (memberName == "value" {
            state.push<float>(value);
        } else if (memberName == "setValue") {
            state.push<Method>([](Counter& object, LuaStateView& state) -> int {
                object.setValue(state.get<float>(2));
                return 0;
            });
        } else if (memberName == "inc") {
            state.push<Method>([](Counter& object, LuaStateView& state) -> int {
            	object.inc();
                return 0;
            });
        } else {
        	result = 0;
        }
        return result;
    }

    static std::string luaToStringImpl(Counter& object) {
        std::ostringstream result;
        result << "{value=" << object.getValue() << "}";
        return result.str();
    }
}
```

### LuaVirtualClass

This binding method consists in deriving the C++ bound type, and implement the virtual methods. In this case, the specializations of [LuaBinding](include/lua/LuaBinding.hpp) are already implemented in the library: you just have to include the appropriate headers:
- [lua/bindings/luaVirtualClass/base.hpp](include/lua/bindings/luaVirtualClass/base.hpp) to push/get `SomeType` objects.
- [lua/bindings/luaVirtualClass/pointers.hpp](include/lua/bindings/luaVirtualClass/base.hpp) to push/get `SomeType*` objects (and [lua/bindings/pointers.hpp](include/lua/bindings/pointers.hpp) for `SomeType**`).
- [lua/bindings/luaVirtualClass/shared_ptr.hpp](include/lua/bindings/luaVirtualClass/shared_ptr.hpp) to push/get `std::shared_ptr<SomeType>` objects.

The example class in the previous section could be written as such:

```
#include <memory>
#include "lua/types/LuaVirtualClass.hpp"

class Counter : public LuaVirtualClass {
private:
	int value;
public:
	Counter(int initVal) : value(initVal) {}

    int luaIndex(const std::string& memberName, LuaStateView& state) override {
        using Method = LuaMethod<Counter>;
        int result = 1;
        if (memberName == "value" {
            state.push<float>(value);
        } else if (memberName == "setValue") {
            state.push<Method>([](Counter& object, LuaStateView& state) -> int {
                object.setValue(state.get<float>(2));
                return 0;
            });
        } else if (memberName == "inc") {
            state.push<Method>([](Counter& object, LuaStateView& state) -> int {
            	object.inc();
                return 0;
            });
        } else {
        	result = 0;
        }
        return result;
    }

    static std::unique_ptr<Counter> getFromTable(LuaTable& table) {
        int val = table.get<LuaNativeString, float>("value");
        return std::make_unique<Counter>(val);
    }

    void inc() { value++; }
    void setValue(int newVal) { value = newVal; }
    int getValue() const { return value; }
};
```

Note that the table constructor, `getFromTable` can return a `std::unique_ptr<Counter>` instead of a plain object: this enables the table constructor to return a derived type if needed.
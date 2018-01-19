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

#ifndef LUABINDINGVIRTUALCLASS_HPP
#define LUABINDINGVIRTUALCLASS_HPP

#include <type_traits>

#include "LuaBinding.hpp"
#include "LuaBindingFunc.hpp"
#include "LuaDefaultBinding.hpp"
#include "LuaStateView.hpp"
#include "LuaVirtualClass.hpp"
#include "LuaWrapFunction.hpp"

template<>
class LuaBinding<LuaVirtualClass*(*)(void*)> : public LuaDefaultBinding<LuaVirtualClass*(*)(void*)> {
public:
    static const std::string& luaClassName() {
        static const std::string className("LuaVirtualClass*(*)(void*)");
        return className;
    }
};

/**
 * See LuaBinding in LuaBinding.hpp.
 *
 * This template defines the binding of all types deriving from LuaVirtualClass.
 * The binding of these types supports polymorphism:
 *
 * <code>
 *   // given type hierarchy Derived -> Parent -> LuaVirtualClass
 *   state.push<Derived>(...);
 *   // The following getRef work:
 *   state.getRef<Derived>(-1);
 *   state.getRef<Parent>(-1);
 *   state.getRef<LuaVirtualClass>(-1);
 * </code>
 */
template<typename T>
class LuaBinding<T, typename std::enable_if<std::is_base_of<LuaVirtualClass,T>::value>::type> {
private:
    template<typename Type>
    using enable_if_copy_constructible = typename std::enable_if<std::is_copy_constructible<Type>::value, Type>::type;

    /**
     * Upcast a pointer from base T to LuaVirtualClass.
     *
     * This function is stored in the metatable of userdata of type T. It enables C++
     * code to turn the userdatum in the Lua stack (of unknown type void*) into a
     * known virtual type, from which it is possible to downcast.
     *
     * @param userData Pointer to a userdatum of type T.
     *
     * @return A pointer to the userdatum upcasted to LuaVirtualClass*.
     */
    static LuaVirtualClass* luaCastPtr(void* userData) {
        T* basePtr = reinterpret_cast<T*>(userData);
        return static_cast<LuaVirtualClass*>(basePtr);
    }

    /**
     * Implementation of Lua metamethod "__gc" for this type.
     *
     * @param state State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaDelete(LuaStateView& state) {
        LuaVirtualClass& object(state.getRef<LuaVirtualClass>(1));
        object.~LuaVirtualClass();
        return 0;
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state, LuaVirtualClass* object) {
        bool newTable = state.newMetatable(object->luaClassName());
        if (newTable) {
            state.push<LuaVirtualClass*(*)(void*)>(luaCastPtr);
            state.setField(-2, "castPtr");
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaDelete>);
            state.setField(-2,"__gc");
            // TODO: get methods & fields from object.
        }
    }
public:
    /**
     * Pushes a new object of type T into Lua.
     *
     * @tparam ArgsType Types of the constructor arguments for type T.
     *
     * @param state Lua state in which the push is done.
     * @param constructorArgs Arguments for the constructor of type T.
     */
    template<typename... ArgsType>
    static void push(LuaStateView& state, ArgsType&&... constructorArgs) {
        LuaVirtualClass* object = state.newObject<T>(std::forward<ArgsType>(constructorArgs)...);
        pushMetatable(state, object);
        state.setMetatable(-2);
    }

    /**
     * Get a reference to an object of type T at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired T object, if the object in the stack is of this type.
     */
    static T& getRef(LuaStateView& state, int stackIndex) {
        void* basePtr = state.checkUserData(stackIndex);

        // Upcast to LuaVirtualClass using the function in the metatable.
        if (!state.pushMetafield(stackIndex, "castPtr")) {
            std::string errorMsg = "Expected LuaVirtualClass or derived type";
            state.throwArgError(stackIndex, errorMsg);
        }
        LuaVirtualClass*(*castPtr)(void*) = state.get<LuaVirtualClass*(*)(void*)>(-1);
        state.pop(1);
        LuaVirtualClass* luaVirtual = castPtr(basePtr);

        // Downcast to type T*.
        T* result = dynamic_cast<T*>(luaVirtual);
        if (result == nullptr) {
            std::string errorMsg = "Expected ";
            errorMsg = errorMsg + typeid(T).name() + ", got " + luaVirtual->luaClassName();
            state.throwArgError(stackIndex, errorMsg);
        }

        return *result;
    }

    /**
     * Gets a copy of the object of type T at the given index from the stack.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return A copy of the object at the given index, if it is of type T (or derived).
     */
    template<typename U=T>
    static enable_if_copy_constructible<U> get(LuaStateView& state, int stackIndex) {
        return getRef(state, stackIndex);
    }
};

#endif /* LUABINDINGVIRTUALCLASS_HPP */


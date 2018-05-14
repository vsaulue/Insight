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

#include "lua/LuaBinding.hpp"
#include "lua/LuaDereferencer.hpp"
#include "lua/LuaStateView.hpp"

#include "lua/bindings/helpers/LuaDefaultClassName.hpp"
#include "lua/bindings/helpers/LuaDefaultDereferenceGet.hpp"
#include "lua/bindings/helpers/LuaWrapFunction.hpp"

#include "lua/types/LuaDereferenceGetter.hpp"
#include "lua/types/LuaCFunction.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "lua/types/LuaUpcaster.hpp"
#include "lua/types/LuaVirtualClass.hpp"

/**
 * Specialization for types derived from LuaVirtualClass.
 *
 * All types derived from LuaVirtualClass have LuaVirtualClass as a basetype.
 *
 * @tparam BoundType Type to dereference (must be derived from LuaVirtualClass).
 */
template<typename BoundType>
class LuaDereferencer<BoundType, typename std::enable_if<std::is_base_of<LuaVirtualClass, BoundType>::value>::type> {
public:
    using basetype = LuaVirtualClass;

    static basetype& dereferenceGetter(LuaStateView& state, int stackIndex) {
        return state.getRef<basetype>(stackIndex);
    }

    static basetype& dereference(BoundType& ref) {
        return ref;
    }
};

/**
 * SFINAE constrcut returning type void if the parameter is derived from LuaVirtualClass.
 * @tparam T Type to check for inheritance from LuaVirtualClass.
 */
template<typename T>
using void_t_if_LuaVirtualClass = typename std::enable_if<std::is_base_of<LuaVirtualClass,T>::value>::type;

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
class LuaBinding<T, void_t_if_LuaVirtualClass<T>> : public LuaDefaultClassName<T> {
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
     * Implementation of Lua metamethod "__index" for this type.
     *
     * @param state State calling this function.
     *
     * @return The number of return values of this Lua function.
     */
    static int luaIndex(LuaStateView& state) {
        LuaVirtualClass& object = state.getRef<LuaVirtualClass>(1);
        std::string memberName(state.get<LuaNativeString>(2));

        return object.luaIndex(memberName, state);
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        bool newTable = state.newMetatable(LuaBinding<T>::luaClassName());
        if (newTable) {
            state.push<LuaUpcaster<LuaVirtualClass>>(luaCastPtr);
            state.setField(-2, "castPtr");
            state.push<LuaCFunction>(luaWrapFunction<luaDelete>);
            state.setField(-2,"__gc");
            state.push<LuaCFunction>(luaWrapFunction<luaIndex>);
            state.setField(-2,"__index");
            state.push<LuaDereferenceGetter<LuaVirtualClass>>(LuaBinding<LuaVirtualClass>::getRef);
            state.setField(-2,"dereferenceGetter");
        }
    }

    /**
     * Downcast an Lua argument of type LuaVirtualClass* into T*.
     *
     * @param basePtr Pointer to downcast.
     * @param state Current Lua state (used for error reporting).
     * @param stackIndex Index of the argument in the stack (used for error reporting).
     * @return basePtr downcasted into type T*.
     */
    static T* downcastArg(LuaVirtualClass* basePtr, LuaStateView& state, int stackIndex) {
        T* result = dynamic_cast<T*>(basePtr);
        if (result == nullptr) {
            std::string errorMsg = "Expected ";
            errorMsg+= LuaBinding<T>::luaClassName() + ", got " + state.getTypename(stackIndex);
            state.throwArgError(stackIndex, errorMsg);
        }
        return result;
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
        pushMetatable(state);
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
        LuaUpcaster<LuaVirtualClass> upcast = state.get<LuaUpcaster<LuaVirtualClass>>(-1);
        state.pop(1);
        LuaVirtualClass* luaVirtual = upcast(basePtr);

        // Downcast to type T*.
        return *downcastArg(luaVirtual, state, stackIndex);
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

    /**
     * Dereference the object at the specified index into a T&.
     *
     * @param state State containing the Lua stack.
     * @param stackIndex Index in the stack of the item to convert.
     *
     * @return A reference to base object stored at the given index.
     */
    static T& dereferenceGet(LuaStateView& state, int stackIndex) {
        LuaVirtualClass* basePtr = &luaDefaultDereferenceGet<LuaVirtualClass>(state, stackIndex);
        return *downcastArg(basePtr, state, stackIndex);
    }
};

// Defined in lua/bindings/luaVirtualClass/pointers.hpp.
template<typename T>
class LuaPointerBinding<T, typename std::enable_if<std::is_base_of<LuaVirtualClass, T>::value>::type>;

#endif /* LUABINDINGVIRTUALCLASS_HPP */


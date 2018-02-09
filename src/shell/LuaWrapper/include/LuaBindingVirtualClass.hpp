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
#include "LuaDefaultClassName.hpp"
#include "LuaDefaultDereferenceGet.hpp"
#include "LuaDereferenceGetter.hpp"
#include "LuaDereferencer.hpp"
#include "LuaPointerBindings.hpp"
#include "LuaStateView.hpp"
#include "LuaUpcaster.hpp"
#include "LuaVirtualClass.hpp"
#include "LuaWrapFunction.hpp"

/**
 * Specialization for types derived from LuaVirtualClass.
 *
 * All types derived from LuaVirtualClass have LuaVirtualClass as a basetype.
 *
 * @tparam BindedType Type to dereference (must be derived from LuaVirtualClass).
 */
template<typename BindedType>
class LuaDereferencer<BindedType, typename std::enable_if<std::is_base_of<LuaVirtualClass, BindedType>::value>::type> {
public:
    using basetype = LuaVirtualClass;

    static basetype& dereferenceGetter(LuaStateView& state, int stackIndex) {
        return state.getRef<basetype>(stackIndex);
    }

    static basetype& dereference(BindedType& ref) {
        return ref;
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
class LuaBinding<T, typename std::enable_if<std::is_base_of<LuaVirtualClass,T>::value>::type> : public LuaDefaultClassName<T> {
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
        std::string memberName(state.get<const char*>(2));

        return object.luaIndex(memberName, state);
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state, LuaVirtualClass* object) {
        bool newTable = state.newMetatable(LuaBinding<T>::luaClassName());
        if (newTable) {
            state.push<LuaUpcaster<LuaVirtualClass>>(luaCastPtr);
            state.setField(-2, "castPtr");
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaDelete>);
            state.setField(-2,"__gc");
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaIndex>);
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

/**
 * Specialization for pointer to any class deriving from LuaVirtualClass.
 *
 * This binding supports polymorphism for the get() method:
 * <code>
 * LuaState state;
 *
 * // Given the type hierarchy: Derived -> Base -> LuaVirtualClass
 * Derived derived();
 * state.push<Base*>(&derived);
 *
 * // all following LuaStateView.get() calls works:
 * Base toBase = state.get<Base*>(-1);
 * Derived toDerived = state.get<Derived*>(-1); // implicit downcast performed.
 * LuaVirtualClass *toVirtual = state.get<LuaVirtualClass*>(-1); // implicit upcast performed.
 * </code>
 */
template<typename PointedType>
class LuaPointerBinding<PointedType, typename std::enable_if<std::is_base_of<LuaVirtualClass, PointedType>::value>::type> : LuaDefaultClassName<PointedType*> {
private:

    /**
     * Upcast a pointer from base PointedType to LuaVirtualClass.
     *
     * This function is stored in the metatable of userdata of type PointedType*. It enables C++
     * code to turn the userdatum in the Lua stack (of unknown type void*) into a
     * known virtual type, from which it is possible to downcast.
     *
     * @param userData Pointer to a userdatum of type PointedType.
     *
     * @return A pointer to the userdatum upcasted to LuaVirtualClass*.
     */
    static LuaVirtualClass* luaCastPtr(void* userdata) {
        PointedType* basePtr = reinterpret_cast<PointedType*>(userdata);
        return static_cast<LuaVirtualClass*>(basePtr);
    }

    /**
     * Implementation of Lua __index metamethod.
     *
     * @param object Object on which the index function is called.
     * @param memberName Name of the field requested in object by Lua.
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    static int luaIndex(LuaStateView& state) {
        LuaVirtualClass* object = state.get<LuaVirtualClass*>(1);
        std::string memberName(state.get<const char*>(2));

        return luaIndexImpl(object, memberName, state);
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state, LuaVirtualClass* object) {
        bool newTable = state.newMetatable(LuaBinding<PointedType*>::luaClassName());
        if (newTable) {
            state.push<LuaUpcaster<LuaVirtualClass>>(luaCastPtr);
            state.setField(-2, "castPtr*");
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaIndex>);
            state.setField(-2, "__index");
            state.push<LuaDereferenceGetter<LuaVirtualClass>>(LuaDereferencer<PointedType*>::dereferenceGetter);
            state.setField(-2, "dereferenceGetter");
        }
    }
public:
    /**
     * Implementation of Lua __index metamethod.
     *
     * This LuaBinding<BindedType> defines a luaIndexImpl so that other bindings can call it (ex: pointer bindings).
     *
     * @param object Object on which the index function is called.
     * @param memberName Name of the field requested in object by Lua.
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    static int luaIndexImpl(LuaVirtualClass* object, const std::string& memberName, LuaStateView& state) {
        if (object == nullptr) {
            state.throwArgError(1, "Cannot get field or method from a null pointer.");
        }
        return object->luaIndex(memberName, state);
    }

    /**
     * Pushes a pointer of type PointedType* into Lua.
     *
     * @param state Lua state in which the push is done.
     * @param value Initial value of the pointer stored into Lua.
     */
    static void push(LuaStateView& state, PointedType* value) {
        state.newObject<PointedType*>(value);
        pushMetatable(state, value);
        state.setMetatable(-2);
    }

    /**
     * Gets a copy of the pointer at the given index from the stack.
     *
     * This function supports polymorphism: if the element at the specified index
     * is not exactly of type PointedType*, this function will look at valid
     * downcasts & upcasts in order to convert it to a PointedType*.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired function pointer, if the object in the stack is of this type.
     */
    static PointedType* get(LuaStateView& state, int stackIndex) {
        void* userdata = state.checkUserData(stackIndex);

        if (!state.pushMetafield(stackIndex, "castPtr*")) {
            std::string errorMsg = "Expected pointer to LuaVirtualClass or derived type";
            state.throwArgError(stackIndex, errorMsg);
        }
        void* rawPtr = *reinterpret_cast<void**> (userdata);
        PointedType* result = nullptr;
        if (rawPtr != nullptr) {
            LuaUpcaster<LuaVirtualClass> upcast = state.get<LuaUpcaster<LuaVirtualClass>>(-1);
            state.pop(1);
            LuaVirtualClass* luaVirtual = upcast(rawPtr);

            result = dynamic_cast<PointedType*>(luaVirtual);
            if (result == nullptr) {
                std::string errorMsg = "Expected ";
                errorMsg = errorMsg + LuaBinding<PointedType*>::luaClassName() + ", got " + state.getTypename(stackIndex) + "*";
                state.throwArgError(stackIndex, errorMsg);
            }
        }
        return result;
    }

    /**
     * Get a reference to the pointer to PointedType at the given index from the stack.
     *
     * This function does not support polymorphism: LuaStateView::getRef<PointedType*>
     * can work only on a lua value constructed by LuaStateView::push<PointedType*>.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired reference to a PointedType* object.
     */
    static PointedType*& getRef(LuaStateView& state, int stackIndex) {
        PointedType** result = state.checkUserData<PointedType*>(stackIndex, LuaBinding<PointedType*>::luaClassName());
        return *result;
    }
};

#endif /* LUABINDINGVIRTUALCLASS_HPP */


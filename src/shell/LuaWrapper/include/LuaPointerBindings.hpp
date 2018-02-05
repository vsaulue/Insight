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

#ifndef LUAPOINTERBINDINGS_HPP
#define LUAPOINTERBINDINGS_HPP

#include <type_traits>

#include "LuaBinding.hpp"
#include "LuaDefaultBinding.hpp"
#include "LuaStateView.hpp"
#include "LuaVirtualClass.hpp"
#include "LuaWrapFunction.hpp"

/**
 * Class defining Lua bindings for the C++ type PointedType*.
 *
 * This class should define the static methods of LuaBinding<PointedType*>.
 *
 * @tparam PointedType Type which pointer will be binded in Lua.
 * @tparam Enable Unused type (used only to enable a specialisation under specific conditions).
 */
template<typename PointedType, typename Enabled=void>
class LuaPointerBinding;

/** 
 * See LuaBinding in LuaBinding.hpp. 
 * 
 * Default template specialization for pointer types.
 */
template <typename PointedType>
class LuaBinding<PointedType*> : public LuaPointerBinding<PointedType> {

};

// Default implementation, for pointers to non-polymorphic types.
template<typename PointedType>
class LuaPointerBinding<PointedType, std::void_t<decltype(LuaBinding<PointedType>::luaClassName)>> : public LuaDefaultBinding<PointedType*> {
public:
    /**
     * Gets the name of the metatable of this type.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className(LuaBinding<PointedType>::luaClassName() + "*");
        return className;
    }

    /**
     * Implementation of Lua __index metamethod (wrapped by LuaDefaultBinding).
     *
     * @param object Object on which the index function is called.
     * @param memberName Name of the field requested in object by Lua.
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    template<typename T=PointedType>
    static typename std::enable_if<LuaDefaultIndex<T>::hasIndex, int>::type luaIndexImpl(PointedType* object, const std::string& memberName, LuaStateView& state) {
        if (object == nullptr) {
            state.throwArgError(1, "Cannot get field or method from a null pointer.");
        }
        return LuaBinding<PointedType>::luaIndexImpl(*object, memberName, state);
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
class LuaPointerBinding<PointedType, typename std::enable_if<std::is_base_of<LuaVirtualClass, PointedType>::value>::type> {
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
        bool newTable = state.newMetatable(luaClassName());
        if (newTable) {
            state.push<LuaVirtualClass*(*)(void*)>(luaCastPtr);
            state.setField(-2, "castPtr*");
            state.push<int(*)(lua_State*)>(luaWrapFunction<luaIndex>);
            state.setField(-2, "__index");
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
     * Gets the name of the metatable of this type.
     *
     * @return The name of this C++ type in Lua.
     */
    static const std::string& luaClassName() {
        static const std::string className(std::string(typeid(PointedType).name())+"*");
        return className;
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
            LuaVirtualClass*(*castPtr)(void*) = state.get < LuaVirtualClass * (*)(void*) >(-1);
            state.pop(1);
            LuaVirtualClass* luaVirtual = castPtr(rawPtr);

            result = dynamic_cast<PointedType*>(luaVirtual);
            if (result == nullptr) {
                std::string errorMsg = "Expected ";
                errorMsg = errorMsg + typeid(PointedType).name() + "*, got " + luaVirtual->luaClassName() + "*";
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
        PointedType** result = state.checkUserData<PointedType*>(stackIndex, luaClassName());
        return *result;
    }
};

#endif /* LUAPOINTERBINDINGS_HPP */


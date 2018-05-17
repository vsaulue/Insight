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

#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <memory>
#include <type_traits>

#include "lua/bindings/helpers/LuaDefaultClassName.hpp"
#include "lua/bindings/helpers/LuaDefaultDelete.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/bindings/luaVirtualClass/helpers/LuaVirtualTraits.hpp"
#include "lua/LuaBinding.hpp"
#include "lua/types/LuaVirtualClass.hpp"

/**
 * Specialization for std::shared_ptr to any class deriving from LuaVirtualClass.
 *
 * This binding supports polymorphism for the get() method:
 * <code>
 * LuaState state;
 *
 * // Given the type hierarchy: Derived -> Base -> LuaVirtualClass
 * Derived derived();
 * state.push<shared_ptr<Base>>(&derived);
 *
 * // all following LuaStateView.get() calls works:
 * shared_ptr<Base> toBase = state.get<shared_ptr<Base>>(-1);
 * shared_ptr<Derived> toDerived = state.get<shared_ptr<Derived>>(-1); // implicit downcast performed.
 * shared_ptr<LuaVirtualClass> toVirtual = state.get<shared_ptr<LuaVirtualClass>>(-1); // implicit upcast performed.
 * </code>
 */
template<typename PointedType>
class LuaSharedPtrBinding<PointedType, void_t_if_LuaVirtualClass<PointedType>> : public LuaDefaultClassName<std::shared_ptr<PointedType>> {
private:
    /** C++ type bound to Lua in this class. */
    using BoundType = std::shared_ptr<PointedType>;
    /** Lua traits of the class pointed by the bound shared_ptr. */
    using Traits = LuaVirtualTraits<PointedType>;

    /**
     * Upcast a shared_ptr<PointedType> to shared_ptr<LuaVirtualClass>.
     *
     * This function is stored in the metatable of userdata of type shared_ptr<PointedType>. It enables C++
     * code to turn the userdatum in the Lua stack (of unknown type void*) into a shared_ptr of a
     * known virtual type, from which it is possible to downcast.
     *
     * @param userData Pointer to a userdatum of type shared_ptr<PointedType>.
     *
     * @return A pointer to the userdatum upcasted to shared_ptr<LuaVirtualClass>.
     */
    static std::shared_ptr<LuaVirtualClass> luaCastPtr(void* userdata) {
        BoundType* basePtr = reinterpret_cast<BoundType*>(userdata);
        return std::shared_ptr<LuaVirtualClass>(*basePtr);
    }

    /**
     * Implementation of Lua __index metamethod.
     *
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    static int luaIndex(LuaStateView& state) {
        std::shared_ptr<LuaVirtualClass>& object = state.getRef<std::shared_ptr<LuaVirtualClass>>(1);
        std::string memberName(state.get<LuaNativeString>(2));

        return luaIndexImpl(object, memberName, state);
    }

    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        bool newTable = state.newMetatable(LuaBinding<BoundType>::luaClassName());
        if (newTable) {
            state.push<LuaUpcaster<std::shared_ptr<LuaVirtualClass>>>(luaCastPtr);
            state.setField(-2, "castSharedPtr");
            state.push<LuaCFunction>(luaWrapFunction<LuaDefaultDelete<BoundType>::luaDelete>);
            state.setField(-2, "__gc");
            state.push<LuaCFunction>(luaWrapFunction<luaIndex>);
            state.setField(-2, "__index");
            state.push<LuaDereferenceGetter<LuaVirtualClass>>(LuaDereferencer<BoundType>::dereferenceGetter);
            state.setField(-2, "dereferenceGetter");
        }
    }

    /**
     * Get a copy of a userdatum in the stack at the given index.
     * @param state Lua stack containing the item.
     * @param stackIndex Index of the item on the stack.
     * @return A copy of the userdatum on the stack.
     */
    static BoundType getUserData(LuaStateView& state, int stackIndex) {
        void* userdata = state.checkUserData(stackIndex);

        if (!state.pushMetafield(stackIndex, "castSharedPtr")) {
            std::string errorMsg = "Expected shared_ptr to LuaVirtualClass or derived type";
            state.throwArgError(stackIndex, errorMsg);
        }
        LuaUpcaster<std::shared_ptr<LuaVirtualClass>> upcast = state.get<LuaUpcaster<std::shared_ptr<LuaVirtualClass>>>(-1);
        state.pop(1);
        std::shared_ptr<LuaVirtualClass> luaVirtual = upcast(userdata);

        BoundType result = nullptr;
        if (userdata != nullptr) {
            result = std::dynamic_pointer_cast<PointedType>(luaVirtual);
            if (result.get() == nullptr) {
                std::string errorMsg = "Expected ";
                errorMsg = errorMsg + LuaBinding<BoundType>::luaClassName() + ", got " + state.getTypename(stackIndex);
                state.throwArgError(stackIndex, errorMsg);
            }
        }
        return result;
    }

    /** True if <code>static T getFromTable(...);</code> can be turned into a constructor form Lua table. */
    static constexpr bool can_getFromTable = Traits::has_getFromTable_T && Traits::is_copy_or_move_constructible;
    /** True if BoundType can be constructed from a Lua table. */
    static constexpr bool has_table_constructor = can_getFromTable || Traits::has_getFromTable_ptr_T;

    /**
     * Constructs a BoundType object from a Lua table in the stack.
     * @param table Lua table read to construct the C++ object.
     * @return A C++ object constructed from the given Lua table.
     */
    template<typename T=BoundType>
    static typename std::enable_if<has_table_constructor,T>::type getFromTable(LuaTable& table) {
        if constexpr (Traits::has_getFromTable_ptr_T) {
            return BoundType(PointedType::luaGetFromTable(table));
        } else if constexpr (can_getFromTable) {
            return std::make_shared<PointedType>(PointedType::luaGetFromTable(table));
        }
    }
public:
    /**
     * Implementation of Lua __index metamethod.
     *
     * This LuaBinding<BoundType> defines a luaIndexImpl so that other bindings can call it (ex: pointer bindings).
     *
     * @param object Object on which the index function is called.
     * @param memberName Name of the field requested in object by Lua.
     * @param state Lua state requesting the field.
     * @return The number of values returned to Lua.
     */
    static int luaIndexImpl(std::shared_ptr<LuaVirtualClass>& object, const std::string& memberName, LuaStateView& state) {
        LuaVirtualClass* ptr = object.get();
        if (ptr == nullptr) {
            state.throwArgError(1, "Cannot get field or method from a null pointer.");
        }
        return ptr->luaIndex(memberName, state);
    }

    /**
     * Pushes a shared_ptr<PointedType> object into Lua.
     *
     * @param state Lua state in which the push is done.
     * @param constructorArgs Arguments passed to the constructor of BoundType.
     * @tparam ArgsType Type of the arguments of BoundType constructor.
     */
    template<typename... ArgsType>
    static void push(LuaStateView& state, ArgsType&&... constructorArgs) {
        state.newObject<BoundType>(std::forward<ArgsType>(constructorArgs)...);
        pushMetatable(state);
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
    static BoundType get(LuaStateView& state, int stackIndex) {
        if (state.isTable(stackIndex)) {
            if constexpr (has_table_constructor) {
                LuaTable table(state, stackIndex);
                return getFromTable(table);
            } else {
                std::string msg = std::string("Type ") + LuaBinding<BoundType>::luaClassName() + " can't be constructed from an Lua table.";
                state.throwArgError(stackIndex, msg);
            }
        }
        return getUserData(state, stackIndex);
    }

    /**
     * Get a reference to the pointer to PointedType at the given index from the stack.
     *
     * This function does not support polymorphism: LuaStateView::getRef<BoundType>
     * can work only on a lua value constructed by LuaStateView::push<BoundType>.
     *
     * @param state State where the lookup is done.
     * @param stackIndex Index in the Lua stack to search.
     *
     * @return The desired reference to a PointedType* object.
     */
    static BoundType& getRef(LuaStateView& state, int stackIndex) {
        BoundType* result = state.checkUserData<BoundType>(stackIndex, LuaBinding<BoundType>::luaClassName());
        return *result;
    }
};

#endif /* SHARED_PTR_HPP */

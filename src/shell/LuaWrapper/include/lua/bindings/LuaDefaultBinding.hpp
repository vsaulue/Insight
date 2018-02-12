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

#ifndef LUADEFAULTBINDING_HPP
#define LUADEFAULTBINDING_HPP

#include "lua/LuaDereferencer.hpp"
#include "lua/LuaStateView.hpp"

#include "lua/bindings/LuaBasicBinding.hpp"
#include "lua/bindings/helpers/LuaDefaultDereferenceGet.hpp"

#include "lua/types/LuaDereferenceGetter.hpp"

/**
 * Provides default implementations of some functions of LuaBinding.
 *
 * This will define the following functions for Luabinding:
 * - push
 * - getRef
 * - get (if BoundType is copy constructible).
 * - dereferenceGet (if BoundType is a "base" type).
 *
 * For reference types, it will add dereferencing functions in the metatable. This enables LuaMethod<LuaBasetype<BoundType>>
 * to work on an object of type BoundType.
 *
 * LuaBinding<BoundType> can be derived from this object.
 *
 * Optional static methods: same as LuaBasicBinding<BoundType>
 *
 * @tparam BoundType C++ type that will be bound into a Lua type.
 */
template<typename BoundType>
class LuaDefaultBinding : public LuaBasicBinding<BoundType> {
public:
    template<typename T=BoundType>
    static typename std::enable_if<std::is_same<LuaBasetype<T>,T>::value,T>::type& dereferenceGet(LuaStateView& state, int stackIndex) {
        return luaDefaultDereferenceGet<T>(state, stackIndex);
    }

    /**
     * Pushes a new object of type BoundType into Lua.
     *
     * @tparam ArgsType Types of the constructor arguments for type BoundType.
     *
     * @param state Lua state in which the push is done.
     * @param constructorArgs Arguments for the constructor of type BoundType.
     */
    template<typename... ArgTypes>
    static void push(LuaStateView& state, ArgTypes&&... constructorArgs) {
        state.newObject<BoundType>(std::forward<ArgTypes>(constructorArgs)...);
        pushMetatable(state);
        state.setMetatable(-2);
    }
private:
    /**
     * Pushes (or create) the metatable of this type on the Lua state.
     *
     * @param state State in which to push the metatable.
     */
    static void pushMetatable(LuaStateView& state) {
        const std::string& className = LuaBinding<BoundType>::luaClassName();
        bool newTable = state.newMetatable(className);
        if (newTable) {
            setMetafields(state);
        }
    }
protected:
    static void setMetafields(LuaStateView& state) {
        LuaBasicBinding<BoundType>::setMetafields(state);

        using Dereferencer = LuaDereferencer<BoundType>;
        using Basetype = LuaBasetype<BoundType>;
        state.push<LuaDereferenceGetter<Basetype>>(&Dereferencer::dereferenceGetter);
        state.setField(-2,"dereferenceGetter");
    }
};

#endif /* LUADEFAULTBINDING_HPP */


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

#include "LuaBasicBinding.hpp"
#include "LuaDefaultDereferenceGet.hpp"
#include "LuaDereferenceGetter.hpp"
#include "LuaDereferencer.hpp"
#include "LuaStateView.hpp"

/**
 * Provides default implementations of some functions of LuaBinding.
 *
 * This will define the following functions for Luabinding:
 * - push
 * - getRef
 * - get (if BindedType is copy constructible).
 * - dereferenceGet (if BindedType is a "base" type).
 *
 * For reference types, it will add dereferencing functions in the metatable. This enables LuaMethod<LuaBasetype<BindedType>>
 * to work on an object of type BindedType.
 *
 * LuaBinding<BindedType> can be derived from this object.
 *
 * Optional static methods: same as LuaBasicBinding<BindedType>
 *
 * @tparam BindedType C++ type that will be binded into a Lua type.
 */
template<typename BindedType>
class LuaDefaultBinding : public LuaBasicBinding<BindedType> {
public:
    template<typename T=BindedType>
    static typename std::enable_if<std::is_same<LuaBasetype<T>,T>::value,T>::type& dereferenceGet(LuaStateView& state, int stackIndex) {
        return luaDefaultDereferenceGet<T>(state, stackIndex);
    }

    /**
     * Pushes a new object of type BindedType into Lua.
     *
     * @tparam ArgsType Types of the constructor arguments for type BindedType.
     *
     * @param state Lua state in which the push is done.
     * @param constructorArgs Arguments for the constructor of type BindedType.
     */
    template<typename... ArgTypes>
    static void push(LuaStateView& state, ArgTypes&&... constructorArgs) {
        state.newObject<BindedType>(std::forward<ArgTypes>(constructorArgs)...);
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
        const std::string& className = LuaBinding<BindedType>::luaClassName();
        bool newTable = state.newMetatable(className);
        if (newTable) {
            setMetafields(state);
        }
    }
protected:
    static void setMetafields(LuaStateView& state) {
        LuaBasicBinding<BindedType>::setMetafields(state);

        using Dereferencer = LuaDereferencer<BindedType>;
        using Basetype = LuaBasetype<BindedType>;
        state.push<LuaDereferenceGetter<Basetype>>(&Dereferencer::dereferenceGetter);
        state.setField(-2,"dereferenceGetter");
    }
};

#endif /* LUADEFAULTBINDING_HPP */


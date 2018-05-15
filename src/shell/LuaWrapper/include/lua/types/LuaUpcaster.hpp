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

#ifndef LUAUPCASTER_HPP
#define LUAUPCASTER_HPP

#include "lua/LuaBinding.hpp"
#include "lua/bindings/LuaBasicBinding.hpp"

/**
 * Wraps a function used to upcast a derived object of unkown type into a known base type.
 *
 * This object can be stored inside the metatable of a C++ type, enabling a function reading
 * the Lua stack to convert a userdatum into a known C++ type.
 *
 * @tparam Base Type returned by the upcast function.
 */
template<typename Base>
class LuaUpcaster {
private:
    /** Wrapped function performing the upcast. */
    Base(*upcaster)(void*);
public:
    /**
     * Creates a new LuaUpcaster.
     *
     * @param upcaster Function implementing the upcast operation.
     */
    LuaUpcaster(Base(*upcaster)(void*)) : upcaster(upcaster) {

    }

    /**
     * Upcast the given rax pointer into a pointer to Base*.
     * @param rawPtr Pointer to the userdatum in Lua.
     * @return The smart/raw pointer stored in the userdatum upcasted to Base.
     */
    Base operator()(void* rawPtr) {
        return upcaster(rawPtr);
    }
};

template<typename Base>
class LuaBinding<LuaUpcaster<Base>> : public LuaBasicBinding<LuaUpcaster<Base>> {

};

#endif /* LUAUPCASTER_HPP */

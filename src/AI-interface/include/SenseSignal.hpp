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

#ifndef SENSESIGNAL_HPP
#define SENSESIGNAL_HPP

#include "lua/types/LuaVirtualClass.hpp"

class SenseVisitor;

/**
 * Input signal of an AI.
 *
 * Type eraser for Sense<T> types. The apply(SenseVisitor&) method can be used
 * to access the derived type.
 */
class SenseSignal : public LuaVirtualClass {
public:
    virtual ~SenseSignal() = default;

    /** Apply some algorithm to this object (visitor pattern).
     *
     * @param visitor Algorithm to apply on this object.
     */
    virtual void apply(SenseVisitor& visitor) const = 0;
};

#endif /* SENSESIGNAL_HPP */


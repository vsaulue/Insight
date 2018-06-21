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

#ifndef UNITS_FLOATS_HPP
#define UNITS_FLOATS_HPP

#include <type_traits>

#include "units/framework/UnitWrapper.hpp"

namespace Units {

    // Specialisation for fundamental floating point types.
    template<typename DataType, typename Unit>
    struct UnitWrapper<DataType,Unit,typename std::enable_if<std::is_floating_point<DataType>::value>::type> : public BasicUnitWrapper<DataType,Unit> {
    public:
        using BasicUnitWrapper<DataType,Unit>::BasicUnitWrapper;
    };

    /** Float value with a unit type. */
    template<typename Unit>
    using Float = UnitWrapper<float,Unit>;

    /** Double value with a unit type. */
    template<typename Unit>
    using Double = UnitWrapper<double,Unit>;
}

#endif /* UNITS_FLOATS_HPP */

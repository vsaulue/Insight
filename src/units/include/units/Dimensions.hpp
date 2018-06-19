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

#ifndef UNITS_DIMENSIONS_HPP
#define UNITS_DIMENSIONS_HPP

#include <type_traits>

#include "meta/constexpr_types.hpp"
#include "meta/ListWithExp.hpp"
#include "units/framework/Dimensions.hpp"

namespace Dimensions {
    /** Identifier of time dimension. */
    struct Time {};
    /** Instance of a time dimension type. */
    constexpr FundamentalDim<Time> time;

    /** Identifier of length dimension. */
    struct Length {};
    /** Instance of a length dimension type. */
    constexpr FundamentalDim<Length> length;

    /** Identifier of mass dimension.  */
    struct Mass {};
    /** Instance of a mass dimension type. */
    constexpr FundamentalDim<Mass> mass;

    /** Identifier of angle dimension. */
    struct Angle {};
    /** Instance of an angle dimension type. */
    constexpr FundamentalDim<Angle> angle;
}

#endif /* UNITS_DIMENSIONS_HPP */

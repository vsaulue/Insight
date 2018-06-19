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

#ifndef UNIT_FRAMEWORK_DIMENSIONS_HPP
#define UNIT_FRAMEWORK_DIMENSIONS_HPP

#include "meta//constexpr_types.hpp"
#include "meta/ListWithExp.hpp"

namespace Dimensions {
    using namespace meta::constexpr_int_values;

    /** Type used to represent the dimensions of a variable. */
    template<typename... BasicDims>
    using Dim = meta::ListWithExp<BasicDims...>;

    /** Type used to declare a fundamental dimension. */
    template<typename Identifier>
    using FundamentalDim = Dim<meta::ListWithExpItem<Identifier>>;

    /** No-dimension value. */
    constexpr Dim<> nodim;
}

#endif /* UNIT_FRAMEWORK_DIMENSIONS_HPP */

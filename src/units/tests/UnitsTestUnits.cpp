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


#include <cmath>
#include <limits>

#include "units/Units.hpp"

namespace U=Units;
namespace D=Dimensions;

static constexpr double PI = 3.14159265359;

static constexpr bool almostEqual(double a, double b, double relDiff = 8*std::numeric_limits<double>::epsilon()) {
    double absDiff = std::abs(a-b);
    return (absDiff < relDiff * std::max(std::abs(a),std::abs(b)));
}

static_assert(U::nounit.ratio == 1, "");

// fundamental units & equality.

static_assert(U::meter == U::meter, "");
static_assert(U::meter.ratio == 1, "");

static_assert(U::kilogram != U::radian, "");
static_assert(U::meter != U::nounit, "");

// multiplication

static_assert(U::meter * U::second == U::second * U::meter, "");
static_assert((U::meter * U::second).dim == D::length * D::time, "");

// division

static_assert((U::radian / U::degree).dim == D::nodim, "");
static_assert(almostEqual((U::radian / U::degree).ratio, PI / 180.0), "");

// power

static_assert(U::meter[U::n2] == U::meter * U::meter, "");
static_assert(U::meter[U::n2].ratio == 1, "");

static_assert(U::meter[-U::n2] == U::meter[-U::n1] * U::meter[-U::n1], "");
static_assert(U::meter[-U::n2].ratio == 1, "");

static_assert(U::meter[-U::n2].dim == D::length[-D::n2], "");
static_assert(U::meter[U::n3].dim == D::length[D::n3], "");

// exact match required (simplification of the unit produces exact ratios).
static_assert((U::degree[U::n2] * U::meter / U::degree[U::n2]).ratio == 1, "");

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

#include "units/Dimensions.hpp"

namespace D=Dimensions;

static_assert(D::mass != D::nodim);
static_assert(D::mass == D::mass * D::nodim,"");
static_assert(D::nodim * D::mass == D::mass,"");
static_assert(D::mass * D::time == D::time * D::mass,"");

static_assert(D::mass[D::n1] == D::mass,"");
static_assert(D::mass[D::n2] != D::mass,"");


static_assert(D::mass[D::n3] == D::mass * D::mass * D::mass,"");
static_assert(D::mass[D::n2][D::n2] == D::mass[D::n4],"");
static_assert(D::mass[D::n2] * D::length[D::n1] == D::mass * D::length * D::mass,"");
static_assert((D::mass * D::length)[D::n2] * D::time == D::mass[D::n2] * D::time * D::length[D::n2],"");

static_assert(D::length * D::length[-D::n1] == D::nodim, "");
static_assert(D::length[D::n2] * D::time * D::length[-D::n1] == D::length * D::time, "");

static_assert(D::mass / D::mass == D::nodim, "");
static_assert(D::time[-D::n1] / D::time == D::time[-D::n2], "");

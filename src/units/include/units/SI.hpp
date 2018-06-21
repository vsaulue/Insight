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

#ifndef SI_HPP
#define SI_HPP

#include "units/Units.hpp"
#include "units/UnitSystem.hpp"

namespace Units {
    namespace details {
        /** International system of unit map: Dimension to Unit. */
        struct SITags {
            using Angle = Radian;
            using Length = Meter;
            using Mass = Kilogram;
            using Time = Second;
        };
    }

    /** International system of units. */
    struct SI : public UnitSystem<details::SITags> {

    };
}

/** International system of units. */
using SI = Units::SI;

#endif /* SI_HPP */

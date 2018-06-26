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

#ifndef UNITS_UNITSYSTEM_HPP
#define UNITS_UNITSYSTEM_HPP

#include "units/framework/UnitSystemConverter.hpp"
#include "units/framework/UnitSystemMapper.hpp"
#include "units/Dimensions.hpp"
#include "units/Units.hpp"

namespace Units {
    /**
     * Generates fundamental & derived units from the fundamental dimensions.
     *
     * @tparam Tags A type mapping fundamental dimension identifiers to fundamental
     * units identifiers. See SI.hpp for an example.
     */
    template<typename Tags>
    struct UnitSystem {
        /** Map: Fundamental dimension identifier -> unit identifier. */
        using UnitTags = Tags;
        /** Types used for unitless values. */
        using NoUnit = Unit<>;

        /** Angle unit. */
        using Angle = FundamentalUnit<typename UnitTags::Angle>;
        /** Length unit. */
        using Length = FundamentalUnit<typename UnitTags::Length>;
        /** Mass unit. */
        using Mass = FundamentalUnit<typename UnitTags::Mass>;
        /** Time unit. */
        using Time = FundamentalUnit<typename UnitTags::Time>;

        /** Linear acceleration unit. */
        using Acceleration = decltype(Length() / Time()[n2]);
        /** Angular mass unit. */
        using AngularMass = decltype(Mass() * Length()[n2]);
        /** Angular (rotational) speed. */
        using AngularVelocity = decltype(Angle() / Time());
        /** Density unit. */
        using Density = decltype(Mass() / Length()[n3]);
        /** Force unit. */
        using Force = decltype(Mass() * Length() / Time()[n2]);
        /** Linear speed unit. */
        using Speed = decltype(Length() / Time());
        /** Volume unit. */
        using Volume = decltype(Length()[n3]);
    };

    template<typename System>
    struct UnitSystemMapper<System,D::Angle> {
        using type = typename System::UnitTags::Angle;
    };

    template<typename System>
    struct UnitSystemMapper<System,D::Length> {
        using type = typename System::UnitTags::Length;
    };

    template<typename System>
    struct UnitSystemMapper<System,D::Mass> {
        using type = typename System::UnitTags::Mass;
    };

    template<typename System>
    struct UnitSystemMapper<System,D::Time> {
        using type = typename System::UnitTags::Time;
    };
}

#endif /* UNITS_UNITSYSTEM_HPP */

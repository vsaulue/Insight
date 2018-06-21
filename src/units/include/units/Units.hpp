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

#ifndef UNITS_UNITS_HPP
#define UNITS_UNITS_HPP

#include "Dimensions.hpp"
#include "units/framework/Units.hpp"

namespace Units {
    /** Identifier of meter unit (distance). */
    struct Meter : UnitIdentifier<D::Length> {
        static constexpr char symbol[] = "m";
        static constexpr double ratio = 1;
    };
    /** Instance of the type representing values in meters. */
    constexpr FundamentalUnit<Meter> meter;

    /** Identifier of second unit (time). */
    struct Second : UnitIdentifier<D::Time> {
        static constexpr char symbol[] = "s";
        static constexpr double ratio = 1;
    };
    /** Instance of the type representing values in seconds. */
    constexpr FundamentalUnit<Second> second;

    /** Identifier of kilogram unit (mass). */
    struct Kilogram : UnitIdentifier<D::Mass> {
        static constexpr char symbol[] = "kg";
        static constexpr double ratio = 1;
    };
    /** Instance of the type representing values in kilograms. */
    constexpr FundamentalUnit<Kilogram> kilogram;

    /** Identifier of radian unit (angle). */
    struct Radian : UnitIdentifier<D::Angle> {
        static constexpr char symbol[] = "rad";
        static constexpr double ratio = 1;
    };
    /** Instance of the type representing values in radians. */
    constexpr FundamentalUnit<Radian> radian;

    /** Identifier of degree unit (angle). */
    struct Degree : UnitIdentifier<D::Angle> {
        static constexpr char symbol[] = "deg";
        static constexpr double ratio = Radian::ratio * 180.0 / 3.14159265359;
    };
    /** Instance of the type representing values in degrees (angle). */
    constexpr FundamentalUnit<Degree> degree;
}

#endif /* UNITS_UNITS_HPP */

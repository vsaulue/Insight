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

#ifndef IRRUNITS_HPP
#define IRRUNITS_HPP

#include "units/framework/UnitWrapper.hpp"
#include "units/BulletUnits.hpp"
#include "units/UnitSystem.hpp"

namespace details {
    /** Tag map of the IrrUnits system. */
    struct IrrUnitsTags {
        using Angle = Units::Degree;
        using Length = BulletUnitsTags::Length;
        using Mass = BulletUnitsTags::Mass;
        using Time = BulletUnitsTags::Time;
    };
}

/** Unit system used internally in the Graphic engine. */
struct IrrUnits : public Units::UnitSystem<details::IrrUnitsTags> {

};

/**
 * Converts the input value to a raw datatype in the Graphic engine units.
 *
 * @param input Value to convert.
 * @return A raw datatype, containing the value of the input in IrrUnits system.
 *
 * @tparam DataType Input & output data type.
 * @tparam Unit Input's unit.
 */
template<typename DataType, typename Unit>
DataType toIrrUnits(const Units::UnitWrapper<DataType,Unit>& input) {
    return Units::systemConvert<IrrUnits>(input).value;
}

/**
 * Wraps a raw input value (graphic engine units) in a UnitWrapper type.
 *
 * @param input Raw value from thegraphics engine.
 * @return The input value converted to the desired unit.
 *
 * @tparam ResUnit Unit of the output value. The caller must ensure that the input
 * raw data has the same dimension, expressed in graphics engine units.
 * @tparam DataType Input & output data type.
 */
template<typename ResUnit, typename DataType>
Units::UnitWrapper<DataType,ResUnit> fromIrrValue(const DataType& input) {
    using InputUnit = Units::UnitConverter<IrrUnits,ResUnit>;
    return Units::UnitWrapper<DataType,InputUnit>(input);
}

#endif /* IRRUNITS_HPP */

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

#ifndef BASICUNITWRAPPER_HPP
#define BASICUNITWRAPPER_HPP

#include "units/framework/Units.hpp"

namespace Units {
    /**
     * Base class for UnitWrapper implementations.
     *
     * @tparam C++ type holding the numerical data value.
     * @tparam Type representing the unit of this value.
     */
    template<typename DataType, typename _Unit>
    struct BasicUnitWrapper;

    namespace details {
        /**
         * Converts a value from one unit to another.
         *
         * @param input Input value (with its associated unit).
         * @return The input value in a raw data type (no unit attached), converted to the specified unit.
         *
         * @tparam OutUnit Desired output unit.
         * @tparam InDataType DataType of the input UnitWrapper.
         * @tparam InUnit Unit of the input UnitWrapper.
         */
        template<typename OutUnit, typename InDataType, typename InUnit>
        static auto convert(const BasicUnitWrapper<InDataType,InUnit>& input) {
            using InDim = typename InUnit::Dim;
            using OutDim = typename OutUnit::Dim;
            static_assert(InDim() == OutDim(), "Invalid conversion: different dimensions.");
            static constexpr auto ratio = (OutUnit() / InUnit()).ratio;
            if constexpr (ratio == 1) {
                return input.value;
            } else {
                return ratio * input.value;
            }
        }
    }

    template<typename DataType, typename _Unit>
    struct BasicUnitWrapper {
    protected:
        /**
         * Tests if the unit of this object has the same dimention as another unit.
         * @tparam OtherUnit Unit to compare to the one of this object.
         */
        template<typename OtherUnit>
        struct has_same_dim_as : std::integral_constant<bool, (typename OtherUnit::Dim){} == (typename _Unit::Dim){}> {};
    public:
        /** Unit of the value of this object. */
        using Unit = _Unit;

        /** Numerical value of this object. */
        DataType value;

        /**
         * Constructor forwarding directly to the DataType constructor.
         *
         * No conversion is performed by this constructor. The raw value constructed
         * constructed from the constructor will directly be interpreted as Unit type.
         *
         * @param constructorArgs Arguments of the raw value constructor.
         * @tparam ArgsType Types of the arguments.
         */
        template<typename... ArgsType, typename std::enable_if<std::is_constructible<DataType,ArgsType...>::value,int>::type = 0>
        explicit BasicUnitWrapper(ArgsType&&... constructorArgs) : value(std::forward<ArgsType>(constructorArgs)...) {

        }

        /**
         * Copy constructor.
         *
         * Requires the argument to be of the same dimension as this object. The value
         * will be converted to the unit of this object.
         *
         * @param value Copied value
         */
        template<typename OtherDataType, typename OtherUnit, typename std::enable_if<has_same_dim_as<OtherUnit>::value,int>::type=0>
        BasicUnitWrapper(const BasicUnitWrapper<OtherDataType,OtherUnit>& value)
            : value(details::convert<Unit>(value))
        {

        }

        /**
         * Assignment operator.
         *
         * Requires the argument to be of the same dimension as this object. The value
         * will be converted to the unit of this object.
         *
         * @param other Copied value.
         * @return A reference to this object.
         */
        template<typename OtherDataType, typename OtherUnit, typename std::enable_if<has_same_dim_as<OtherUnit>::value,int>::type=0>
        BasicUnitWrapper& operator=(const BasicUnitWrapper<OtherDataType,OtherUnit>& other) {
            value = details::convert<Unit>(other);
            return *this;
        }
    };
}

#endif /* BASICUNITWRAPPER_HPP */

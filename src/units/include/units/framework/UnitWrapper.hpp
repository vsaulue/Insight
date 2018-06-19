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

#ifndef UNITS_UNITWRAPPER_HPP
#define UNITS_UNITWRAPPER_HPP

#include <type_traits>

#include "units/framework/BasicUnitWrapper.hpp"
#include "units/framework/Units.hpp"

namespace Units {
    /**
     * Object wrapping an existing data type, and a unit known at compile time.
     *
     * Any specialization should derive from BasicUnitWrapper. The UnitWrapper
     * class adds basic implementation of arithmetic & comparisons operators,
     * while allowing users to add specific methods to the wrapper of some datatypes.
     *
     * @tparam _DataType C++ type holding the digital part of the value.
     * @tparam _Unit Type representing the unit of this value.
     * @tparam Unused type (used for SFINAE in specializations).
     */
    template<typename DataType, typename Unit, typename Enable=void>
    struct UnitWrapper;

    namespace details {
        /**
         * Integral constant checking is (T1 *= T2) is well formed.
         *
         * Specialisation failure if false.
         * @tparam T1 Type of the left operand.
         * @tparam T2 Type of the right operand.
         */
        template<typename T1, typename T2>
        using can_mult_eq = std::is_same<void,std::void_t<decltype(std::declval<T1&>() *= std::declval<T2&>())>>;

        /**
         * Integral constant checking is (T1 /= T2) is well formed.
         *
         * Specialisation failure if false.
         * @tparam T1 Type of the left operand.
         * @tparam T2 Type of the right operand.
         */
        template<typename T1, typename T2>
        using can_div_eq = std::is_same<void,std::void_t<decltype(std::declval<T1&>() /= std::declval<T2&>())>>;
    }

    /** UnitWrapper + UnitWrapper operator.
     *
     * The two operands must have the same unit.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the sum of the inputs.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    auto operator+(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        static_assert(Unit1() == Unit2(), "Invalid operation: different units.");
        return UnitWrapper<DataType1,Unit1>(l.value + r.value);
    }

    /**
     * UnitWrapper += UnitWrapper operator.
     *
     * The two operands must have the same dimensions.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A reference to the left operand.
     */
    template<typename LeftType, typename LeftUnit, typename RightType, typename RightUnit>
    auto& operator+=(UnitWrapper<LeftType,LeftUnit>& l, const UnitWrapper<RightType,RightUnit>& r) {
        l.value += details::convert<LeftUnit>(r);
        return l;
    }

    /** UnitWrapper - UnitWrapper operator.
     *
     * The two operands must have the same unit.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the sum of the inputs.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    auto operator-(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        static_assert(Unit1() == Unit2(), "Invalid operation: different units.");
        return UnitWrapper<DataType1,Unit1>(l.value - r.value);
    }

    /**
     * UnitWrapper -= UnitWrapper operator.
     *
     * The two operands must have the same dimensions.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A reference to the left operand.
     */
    template<typename LeftType, typename LeftUnit, typename RightType, typename RightUnit>
    auto& operator-=(UnitWrapper<LeftType,LeftUnit>& l, const UnitWrapper<RightType,RightUnit>& r) {
        l.value -= details::convert<LeftUnit>(r);
        return l;
    }

    /**
     * UnitWrapper * UnitWrapper operator.
     *
     * The unit of the result is the product of the input units.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the product of the inputs.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    auto operator*(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        using RetUnit = decltype(Unit1() * Unit2());
        auto val = l.value * r.value;
        return UnitWrapper<decltype(val),RetUnit>(val);
    }

    /**
     * UnitWrapper * T operator.
     *
     * The right operand is treated as a unitless value. The output's unit is the
     * same as the left operand's.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the product of the inputs.
     */
    template<typename DataType, typename Unit, typename OtherType>
    auto operator*(const UnitWrapper<DataType,Unit>& l, const OtherType& r) ->
        UnitWrapper<decltype(l.value*r),Unit>
    {
        auto val = l.value * r;
        using RetDataType = decltype(val);
        return UnitWrapper<RetDataType,Unit>(val);
    }

    /**
     * T * UnitWrapper operator.
     *
     * The left operand is treated as a unitless value. The output's unit is the
     * same as the right operand's.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the product of the inputs.
     */
    template<typename DataType, typename Unit, typename OtherType>
    auto operator*(const OtherType& l, const UnitWrapper<DataType,Unit>& r) ->
        UnitWrapper<decltype(l * r.value),Unit>
    {
        auto val = l * r.value;
        using RetDataType = decltype(val);
        return UnitWrapper<RetDataType,Unit>(val);
    }

    /**
     * UnitWrapper *= UnitWrapper operator.
     *
     * The right operand must be a dimensionless value.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A reference to the left operand.
     */
    template<typename LeftType, typename LeftUnit, typename RightType, typename RightUnit>
    auto& operator*=(UnitWrapper<LeftType,LeftUnit>& l, const UnitWrapper<RightType,RightUnit>& r) {
        UnitWrapper<RightType,Unit<>> multValue(r);
        l.value *= multValue.value;
        return l;
    }

    /**
     * UnitWrapper *= OtherType operator.
     *
     * The right is treated as a unitless, dimensionless value.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A reference to the left operand.
     */
    template<typename LeftType, typename LeftUnit, typename OtherType, typename std::enable_if<details::can_mult_eq<LeftType,OtherType>::value,int>::type = 0>
    auto& operator*=(UnitWrapper<LeftType,LeftUnit>& l, const OtherType& r) {
        l.value *= r;
        return l;
    }

    /**
     * UnitWrapper / UnitWrapper operator.
     *
     * The unit of the result is the quotient of the input units.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the quotient of the inputs.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    auto operator/(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        using RetUnit = decltype(Unit1() / Unit2());
        auto val = l.value / r.value;
        return UnitWrapper<decltype(val),RetUnit>(val);
    }

    /**
     * UnitWrapper / OtherType operator.
     *
     * The right operand is treated as a unitless value. The output's unit is the
     * same as the left operand's.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the quotient of the inputs.
     */
    template<typename DataType, typename Unit, typename OtherType>
    auto operator/(const UnitWrapper<DataType,Unit>& l, const OtherType& r) ->
        UnitWrapper<decltype(l.value/r),Unit>
    {
        auto val = l.value / r;
        using RetDataType = decltype(val);
        return UnitWrapper<RetDataType,Unit>(val);
    }

    /**
     * OtherType / UnitWrapper operator.
     *
     * The left operand is treated as a unitless value. The output's unit is the
     * same as the right operand's.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A UnitWrapper object equals to the quotient of the inputs.
     */
    template<typename DataType, typename Unit, typename OtherType>
    auto operator/(const OtherType& l, const UnitWrapper<DataType,Unit>& r) ->
        UnitWrapper<decltype(l / r.value),decltype(Unit()[-n1])>
    {
        auto val = l / r.value;
        using RetDataType = decltype(val);
        using RetUnit = decltype(Unit()[-n1]);
        return UnitWrapper<RetDataType,RetUnit>(val);
    }

    /**
     * UnitWrapper /= UnitWrapper operator.
     *
     * The right operand must be a dimensionless value.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A reference to the left operand.
     */
    template<typename LeftType, typename LeftUnit, typename RightType, typename RightUnit>
    auto& operator/=(UnitWrapper<LeftType,LeftUnit>& l, const UnitWrapper<RightType,RightUnit>& r) {
        UnitWrapper<RightType,Unit<>> multValue(r);
        l.value /= multValue.value;
        return l;
    }

    /**
     * UnitWrapper /= OtherType operator.
     *
     * The right is treated as a unitless, dimensionless value.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return A reference to the left operand.
     */
    template<typename LeftType, typename LeftUnit, typename OtherType, typename std::enable_if<details::can_div_eq<LeftType,OtherType>::value,int>::type = 0>
    auto& operator/=(UnitWrapper<LeftType,LeftUnit>& l, const OtherType& r) {
        l.value /= r;
        return l;
    }

    /**
     * Tests for equality between two UnitWrapper objects.
     *
     * The object must have the same unit.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return True if the values are equals, false otherwise.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    bool operator==(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        static_assert(Unit1() == Unit2(), "Cannot test for equality data of different units.");
        return l.value == r.value;
    }

    /**
     * Tests for equality between two UnitWrapper objects.
     *
     * The object must have the same unit.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return True if the values are equals, false otherwise.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    bool operator!=(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        static_assert(Unit1() == Unit2(), "Cannot test for equality data of different units.");
        return l.value != r.value;
    }

    /**
     * UnitWrapper < UnitWrapper operator.
     *
     * The right operand is converted to the unit of the left operand.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return True if l < r.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    bool operator<(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        static_assert(Unit1().dim == Unit2().dim, "Cannot compare values of different dimensions.");
        return l.value < (Unit1() / Unit2()).ratio * r.value;
    }

    /**
     * UnitWrapper <= UnitWrapper operator.
     *
     * The right operand is converted to the unit of the left operand.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return True if l <= r.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    bool operator<=(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        static_assert(Unit1().dim == Unit2().dim, "Cannot compare values of different dimensions.");
        return l.value <= (Unit1() / Unit2()).ratio * r.value;
    }

    /**
     * UnitWrapper > UnitWrapper operator.
     *
     * The right operand is converted to the unit of the left operand.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return True if l > r.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    bool operator>(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        return !(l <= r);
    }

    /**
     * UnitWrapper >= UnitWrapper operator.
     *
     * The right operand is converted to the unit of the left operand.
     *
     * @param l Left operand.
     * @param r Right operand.
     * @return True if l >= r.
     */
    template<typename DataType1, typename Unit1, typename DataType2, typename Unit2>
    bool operator>=(const UnitWrapper<DataType1,Unit1>& l, const UnitWrapper<DataType2,Unit2>& r) {
        return !(l < r);
    }
}

#endif /* UNITS_UNITWRAPPER_HPP */

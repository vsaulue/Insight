/* This file is part of Insight.
 * Copyright (C) 2017 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
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

#ifndef FIXED_POINT_H
#define	FIXED_POINT_H

#include <stdint.h>
#include <type_traits>

/**
 * Signed fixed point.
 */
template<unsigned frac_len>
struct Fixed64 {
    static_assert(frac_len <= 63, "");

    /** Internal value. */
    int64_t value;

    /**
     * Creates an non-initialized fixed point number.
     */
    Fixed64() { }

    /**
     * Creates a fixed point number, initialized from a floating point number.
     *
     * @param doubleVal Initial value of this fixed point number.
     */
    Fixed64(double doubleVal) : value((int64_t) (doubleVal * (static_cast<int64_t>(1) << frac_len))) { }

    /**
     * Creates a fixed point number, by copying the value of another fixed point number.
     *
     * @param fixedVal Initial value of the new fixed point number.
     */
    Fixed64(const Fixed64<frac_len>& fixedVal) : value(fixedVal.value) { }

    /**
     * Adds the argument to the current number.
     *
     * @param rhs The number that will be added to this object.
     *
     * @return A reference to this object.
     */
    Fixed64<frac_len>& operator+=(const Fixed64<frac_len>& rhs) {
        value += rhs.value;
        return *this;
    }

    /**
     * Substracts the argument to the current number.
     *
     * @param rhs The number that will be substracted to this object.
     *
     * @return A reference to this object.
     */
    Fixed64<frac_len>& operator-=(const Fixed64<frac_len>& rhs) {
        value -= rhs.value;
        return *this;
    }

    double toDouble() const {
        return ((double)value) / ((double)(static_cast<int64_t>(1) << frac_len));
    }
};

namespace internal {

    template<unsigned a_frac, unsigned b_frac>
    inline bool _compare(const Fixed64<a_frac>& lhs, const Fixed64<b_frac>& rhs, std::true_type, std::false_type) {
        static_assert(a_frac == b_frac, "This function only works if parameters have the same type.");

        return (lhs.value == rhs.value);
    }

    template<unsigned a_frac, unsigned b_frac>
    inline bool _compare(const Fixed64<a_frac>& lhs, const Fixed64<b_frac>& rhs, std::false_type, std::true_type) {
        static_assert(a_frac > b_frac, "This function only works if the first number has a longer decimal part.");
        static const uint64_t lsb_mask = (~static_cast<uint64_t> (0)) >> (64 - a_frac + b_frac);

        return ((lsb_mask & lhs.value) == 0) && (lhs.value >> (a_frac - b_frac) == rhs.value);
    }

    template<unsigned a_frac, unsigned b_frac>
    inline bool _compare(const Fixed64<a_frac>& lhs, const Fixed64<b_frac>& rhs, std::false_type, std::false_type) {
        static_assert(a_frac < b_frac, "This function only works if the second number has a longer decimal part.");

        return _compare(rhs, lhs, typename std::false_type(), typename std::true_type());
    }
}

/**
 * Logical equality between Fixed64 types.
 *
 * This operator works for arguments of any precision (they can have a different
 * precision).
 *
 * @param lhs First number to compare.
 * @param rhs Second number to compare.
 * @return True if and only if two numbers have the same value.
 */
template<unsigned a_frac, unsigned b_frac>
inline bool operator==(const Fixed64<a_frac>& lhs, const Fixed64<b_frac>& rhs) {
    bool result;

    result = internal::_compare(lhs, rhs, typename std::conditional < a_frac == b_frac, std::true_type, std::false_type>::type(), typename std::conditional < (a_frac > b_frac), std::true_type, std::false_type>::type());

    return result;
}

/**
 * Logical inequality between Fixed64 types.
 *
 * This operator works for arguments of any precision (they can have a different
 * precision).
 *
 * @param lhs First number to compare.
 * @param rhs Second number to compare.
 * @return True if and only if two numbers have different values.
 */
template<unsigned a_frac, unsigned b_frac>
inline bool operator!=(const Fixed64<a_frac>& lhs, const Fixed64<b_frac>& rhs) {
    return !(lhs == rhs);
}

/**
 * Classic addition.
 *
 * This function is only defined for numbers having the same precision.
 *
 * @param lhs First number to addition.
 * @param rhs Second number to addition.
 * @return The sum of the two arguments.
 */
template<unsigned frac>
inline Fixed64<frac> operator+(const Fixed64<frac>& lhs, const Fixed64<frac>& rhs) {
    Fixed64<frac> result;

    result.value = lhs.value + rhs.value;

    return result;
}

/**
 * Classic substraction.
 *
 * This function is only defined for numbers having the same precision.
 *
 * @param lhs Left number in the substraction.
 * @param rhs Right number in the substraction.
 * @return lhs - rhs.
 */
template<unsigned frac>
inline Fixed64<frac> operator-(const Fixed64<frac>& lhs, const Fixed64<frac>& rhs) {
    Fixed64<frac> result;

    result.value = lhs.value - rhs.value;

    return result;
}

/**
 * Classic negation.
 *
 * This function is only defined for numbers having the same precision.
 *
 * Note that the same glitch than int appears when negating the smallest value
 * possible ( - MIN_INT == MIN_INT).
 *
 * @param val Value that must be negated.
 * @return The opposed value to the argument
 */
template<unsigned frac>
inline Fixed64<frac> operator-(const Fixed64<frac>& val) {
    Fixed64<frac> result;

    result.value = -val.value;

    return result;
}

#endif	/* FIXED_POINT_H */


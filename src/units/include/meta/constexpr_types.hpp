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

#ifndef CONSTEXPR_INT_HPP
#define CONSTEXPR_INT_HPP

namespace meta {
    /** Compile time integer. */
    template<int i>
    struct constexpr_int {
        /** Value of this object. */
        static constexpr int value = i;

        /**
         * Computes a const_expr int of opposite value.
         * @return An object of type constexpr_int<-value>.
         */
        constexpr constexpr_int<-i> operator-() const {
            return {};
        }
    };

    namespace constexpr_int_values {
        /** Compile-time integer equals to 1. */
        constexpr constexpr_int<1> n1;
        /** Compile-time integer equals to 2. */
        constexpr constexpr_int<2> n2;
        /** Compile-time integer equals to 3. */
        constexpr constexpr_int<3> n3;
        /** Compile-time integer equals to 4. */
        constexpr constexpr_int<4> n4;
    }

    /**
     * Computes the power of a floating point values at compile time.
     *
     * @param val Base value.
     * @tparam i Exponent.
     */
    template<int i>
    constexpr double pow(double val) {
        if constexpr (i == 0) {
            return 1;
        } else if constexpr (i > 0) {
            return val * pow<i-1>(val);
        } else {
            return 1 / pow<-i>(val);
        }
    }
}

#endif /* CONSTEXPR_INT_HPP */

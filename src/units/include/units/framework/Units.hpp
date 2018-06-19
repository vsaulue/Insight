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

#ifndef UNITS_FRAMEWORK_UNITS_HPP
#define UNITS_FRAMEWORK_UNITS_HPP

#include "Dimensions.hpp"
#include "meta/constexpr_types.hpp"
#include "meta/ListWithExp.hpp"

namespace Units {
    using namespace meta::constexpr_int_values;
    namespace D = Dimensions;

    /** Base class of identifiers of fundamental units. */
    template<typename _DimIdentifier>
    struct UnitIdentifier {
        using DimIdentifier = _DimIdentifier;
    };

    /** Type representing a compound unit (list of fundamental unit with exponents). */
    template<typename... UnitWithExp>
    struct Unit;

    namespace details {
        /** Helper structure used to compute Unit fields. */
        template<typename... UnitWithExp>
        struct UnitImpl;

        // Empty list specialisation.
        template<>
        struct UnitImpl<> {
            using Dim = D::Dim<>;
            static constexpr double ratio = 1;
        };

        // Non-empty list specialisation.
        template<typename Head, typename... Tail>
        struct UnitImpl<Head, Tail...> {
        private:
            /** Head identifier of the unit ListWithExp. */
            using HeadUnitId = typename Head::Identifier;
            /** Head identifier of the dimension ListWithExp. */
            using HeadDimId = typename HeadUnitId::DimIdentifier;
            /** Tail of the dimension ListWithExp. */
            using TailDim = typename UnitImpl<Tail...>::Dim;
        public:
            /** Dimension list of the input unit. */
            using Dim = decltype(TailDim::template addExp<HeadDimId,Head::exp>());
            /** Conversion constant from this unit to standard units. */
            static constexpr double ratio = meta::pow<Head::exp>(HeadUnitId::ratio) * UnitImpl<Tail...>::ratio;
        };

        /** Implementation of ListToUnit helper. */
        template<typename List>
        struct ListToUnitImpl;

        template<typename... UnitWithExp>
        struct ListToUnitImpl<meta::ListWithExp<UnitWithExp...>> {
            using type = Unit<UnitWithExp...>;
        };

        /** Converts a ListWithExp<Items...> type into a Unit<Items...> type. */
        template<typename List>
        using ListToUnit = typename ListToUnitImpl<List>::type;
    }

    template<typename... UnitWithExp>
    struct Unit {
        /** List of fundamental units with associated coefficients. */
        using List = meta::ListWithExp<UnitWithExp...>;
        /** Compile time instance of the unit list with coefficients. */
        static constexpr List list{};
        /** Dimension of this unit. */
        using Dim = typename details::UnitImpl<UnitWithExp...>::Dim;
        /** Compile time instance of the dimension list with coefficients. */
        static constexpr Dim dim{};

        /** Conversion ratio to the corresponding standard unit. */
        static constexpr double ratio = details::UnitImpl<UnitWithExp...>::ratio;

        /**
         * Multiply all the exponents of this unit by the specified value.
         * @param value Compile-time value to multiply the exponents with.
         * @return An instance of the resulting unit type.
         * @tparam i Value to multiply the coefficients with.
         */
        template<int i>
        constexpr auto operator[](meta::constexpr_int<i> value) const {
            return details::ListToUnit<decltype(list[value])>{};
        }
    };

    /**
     * Multiply two units.
     *
     * @param u1 Instance of the first unit type.
     * @param u2 Instance of the second unit type.
     * @return An instance of the resulting unit type.
     */
    template<typename... List1, typename... List2>
    constexpr auto operator*(Unit<List1...> u1 ,Unit<List2...> u2) {
        return details::ListToUnit<decltype(u1.list * u2.list)>{};
    }

    /**
     * Divides two units.
     *
     * @param u1 Instance of the first unit type.
     * @param u2 Instance of the second unit type.
     * @return An instance of the resulting unit type.
     */
    template<typename... List1, typename... List2>
    constexpr auto operator/(Unit<List1...> u1 ,Unit<List2...> u2) {
        return details::ListToUnit<decltype(u1.list / u2.list)>{};
    }

    /**
     * Compares two units.
     *
     * @param u1 Instance of the first unit type.
     * @param u2 Instance of the second unit type.
     * @return True if the two units are the same.
     */
    template<typename... List1, typename... List2>
    constexpr bool operator==(Unit<List1...> u1 ,Unit<List2...> u2) {
        return u1.list == u2.list;
    }

    /**
     * Compares two units.
     *
     * @param u1 Instance of the first unit type.
     * @param u2 Instance of the second unit type.
     * @return True if the two units are different.
     */
    template<typename... List1, typename... List2>
    constexpr bool operator!=(Unit<List1...> u1 ,Unit<List2...> u2) {
        return !(u1 == u2);
    }

    /** Type used to declare a fundamental unit. */
    template<typename Identifier>
    using FundamentalUnit = Unit<meta::ListWithExpItem<Identifier,1>>;

    /** Instance of the type representing unitless values. */
    constexpr Unit<> nounit;
}

#endif /* UNITS_FRAMEWORK_UNITS_HPP */

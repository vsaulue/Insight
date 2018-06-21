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

#ifndef UNITS_UNITSYMBOLS_HPP
#define UNITS_UNITSYMBOLS_HPP

#include <string>
#include <type_traits>

#include "units/framework/Units.hpp"

namespace Units {
    namespace details {
        /**
         * Helper class to generate the symbol of a Unit.
         * @tparam Unit to represent as a symbol.
         */
        template<typename Unit>
        struct UnitSymbolImpl;

        // Specialization for non-empty unit.
        template<typename Head, typename... Tail>
        struct UnitSymbolImpl<Unit<Head,Tail...>> {
            /** Unit identifier of the Head item. */
            using HeadId = typename Head::Identifier;

            /**
             * Append the symbol of this unit to the given string.
             * @param[out] output String to append the symbol to.
             */
            static void appendValue(std::string& output) {
                output += HeadId::symbol;
                if constexpr (Head::exp != 1) {
                    output += "^" + std::to_string(Head::exp);
                }
                output += ".";
                UnitSymbolImpl<Unit<Tail...>>::appendValue(output);
            }

            /**
             * Gets the symbol of this unit.
             * @return The symbol of this unit.
             */
            static std::string getValue() {
                std::string result;
                appendValue(result);
                return result;
            }
        };

        // Specialization for NoUnit.
        template<>
        struct UnitSymbolImpl<Unit<>> {
            // See other UnitSymbolImpl for documentation.
            static void appendValue(std::string& output) {
                if (output.size() > 0) {
                    output.pop_back();
                }
            }

            // See other UnitSymbolImpl for documentation.
            static std::string getValue() {
                return std::string("NoUnit");
            }
        };
    }

    /**
     * Gets a human readable symbol of the unit.
     * @return A string containing the unit symbol.
     * @tparam Unit Unit to represent.
     */
    template<typename Unit>
    const std::string& unitSymbol() {
        static const std::string result(details::UnitSymbolImpl<std::remove_cv_t<Unit>>::getValue());
        return result;
    }
}

#endif /* UNITS_UNITSYMBOLS_HPP */

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

#ifndef UNITSYSTEMCONVERTER_HPP
#define UNITSYSTEMCONVERTER_HPP

#include "units/framework/Dimensions.hpp"
#include "units/framework/UnitSystemMapper.hpp"

namespace Units {
    namespace D = Dimensions;

    namespace details {
        /** Implementation of ProjectListItem. */
        template<typename System, typename DimListItem>
        struct ProjectListItemImpl {
        private:
            /** Dimension identifier of the input item. */
            using DimId = typename DimListItem::Identifier;
            /** Unit identifier of the output item. */
            using UnitId = typename UnitSystemMapper<System,DimId>::type;
        public:
            /** Output (Unit list item). */
            using type = meta::ListWithExpItem<UnitId,DimListItem::exp>;
        };

        /**
         * Converts a dimension list item (DimId,exp) to a unit list item (UnitId, exp).
         * @tparam System System used to map the dimension to a unit identifier.
         * @tparam DimListItem Dimension identifier & exponent.
         */
        template<typename System, typename DimListItem>
        using ProjectListItem = typename ProjectListItemImpl<System,DimListItem>::type;

        /**
         * Gets the unit representing the given dimension in the given system.
         * @tparam System Unit system of the result.
         * @tparam Dim Dimension to express in System units.
         */
        template<typename System, typename Dim>
        struct UnitSystemConverterImpl;

        // Specialization to get the items of the dimension list.
        template<typename System, typename... DimWithExp>
        struct UnitSystemConverterImpl<System, D::Dim<DimWithExp...>> {
            /** Output: Unit<...> type. */
            using type = Unit<ProjectListItem<System,DimWithExp>...>;
        };
    }


    /**
     * Converts the given unit to the desired unit system.
     * @tparam System Unit system of the output unit.
     * @tparam Unit Input unit.
     */
    template<typename System, typename Unit>
    using UnitConverter = typename details::UnitSystemConverterImpl<System,typename Unit::Dim>::type;

    /**
     * Converts a value in a given unit to the equivalent unit of the given system.
     *
     * @param input Input value.
     * @return The same value expressed in units of System.
     *
     * @tparam System Unit system of the output value.
     * @tparam DataType Data type of the input & output value.
     * @tparam Unit Unit of the input value.
     */
    template<typename System, typename DataType, typename Unit>
    auto systemConvert(const UnitWrapper<DataType,Unit>& input) {
        using ResUnit = UnitConverter<System,Unit>;
        return Units::UnitWrapper<DataType,ResUnit>(input);
    }
}

#endif /* UNITSYSTEMCONVERTER_HPP */

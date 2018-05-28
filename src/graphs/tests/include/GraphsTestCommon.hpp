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

#ifndef GRAPHSTESTCOMMON_HPP
#define GRAPHSTESTCOMMON_HPP

#include <algorithm>
#include <vector>

#include <catch.hpp>

/**
 * Looks for an item in a vector, and returns its index.
 *
 * @param vector Looked up vector.
 * @param value Value to search.
 * @return The index ot value in vector.
 *
 * @tparam VectorItemType Type of the elements in the vector.
 * @tparam ValueType Type of the searched value.
 */
template<typename VectorItemType, typename ValueType>
static unsigned getIndex(const std::vector<VectorItemType>& vector, const ValueType& value) {
    auto it = std::find_if(vector.begin(), vector.end(), [&value](const auto& vectorItem) -> bool {
        return value == vectorItem;
    });
    REQUIRE(it != vector.end());
    return std::distance(vector.begin(), it);
}

// Specialisation for container of reference_wrapper.
template<typename VectorItemType, typename ValueType>
static unsigned getIndex(const std::vector<std::reference_wrapper<VectorItemType>>& vector, const ValueType& value) {
    auto it = std::find_if(vector.begin(), vector.end(), [&value](const auto& vectorItem) -> bool {
        return value == vectorItem.get();
    });
    REQUIRE(it != vector.end());
    return std::distance(vector.begin(), it);
}

#endif /* GRAPHSTESTCOMMON_HPP */

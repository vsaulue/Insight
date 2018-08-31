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

#ifndef MATRIX3X3_HPP
#define MATRIX3X3_HPP

#include "btBulletDynamicsCommon.h"

#include "units/framework/BasicUnitWrapper.hpp"
#include "units/framework/UnitWrapper.hpp"
#include "Vector3.hpp"

/**
 * Multiplies a matrix with a scalar.
 * @param leftOperand Scale coefficient.
 * @param rightOperand Matrix to multiply.
 * @return A matrix equals to the product of the two inputs.
 */
inline btMatrix3x3 operator*(double leftOperand, const btMatrix3x3& rightOperand) {
    return rightOperand.scaled(btVector3(leftOperand, leftOperand, leftOperand));
}

namespace Units {
    template<typename Unit>
    struct UnitWrapper<btMatrix3x3,Unit> : public BasicUnitWrapper<btMatrix3x3,Unit> {
    private:
        using Base = BasicUnitWrapper<btMatrix3x3,Unit>;
    public:
        using Base::BasicUnitWrapper;

        /**
         * @brief Create a scaled copy of the matrix
	 * @param scaleVector Values used to scale each column.
         * @tparam OtherUnit unit of the scaling coefficients.
         */
        template<typename OtherUnit>
        auto scaled(const Vector3<OtherUnit>& scaleVector) {
            using ResUnit = decltype(OtherUnit() * Unit());
            return UnitWrapper<btMatrix3x3,ResUnit>(this->value.scaled(scaleVector.value));
        }
    };
}

/**
 * btMatrix3x3 UnitWrapper.
 * @tparam Unit Unit associated to this value.
 */
template<typename Unit>
using Matrix3x3 = Units::UnitWrapper<btMatrix3x3,Unit>;

#endif /* MATRIX3X3_HPP */

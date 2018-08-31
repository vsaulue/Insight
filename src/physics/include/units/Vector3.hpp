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

#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <type_traits>

#include "btBulletDynamicsCommon.h"

#include "units/framework/BasicUnitWrapper.hpp"
#include "units/framework/UnitWrapper.hpp"
#include "units/Scalar.hpp"

namespace Units {
    template<typename Unit>
    struct UnitWrapper<btVector3,Unit> : public BasicUnitWrapper<btVector3,Unit> {
    private:
        using Base = BasicUnitWrapper<btVector3,Unit>;
    public:
        using Base::BasicUnitWrapper;

        /**
         * Creates a new Vector3 from scalar with units.
         *
         * @param x X coordinate value.
         * @param y Y coordinate value.
         * @param z Z coordinate value.
         */
        UnitWrapper(Scalar<Unit> x, Scalar<Unit> y, Scalar<Unit> z) :
            Base(x.value, y.value, z.value)
        {

        }

        /**
         * Gets the X coordinate value.
         * @return The X coordinate value.
         */
        Scalar<Unit> x() const {
            return Scalar<Unit>(this->value.x());
        }

        /**
         * Gets the Y coordinate value.
         * @return The Y coordinate value.
         */
        Scalar<Unit> y() const {
            return Scalar<Unit>(this->value.y());
        }

        /**
         * Gets the Z coordinate value.
         * @return The Z coordinate value.
         */
        Scalar<Unit> z() const {
            return Scalar<Unit>(this->value.z());
        }

        /**
         * Computes the scalar product of this vector with the parameter.
         * @param otherVector The other vector.
         * @return The dot product of this vector with the parameter.
         */
        Scalar<Unit> dot(const btVector3& otherVector) {
            return Scalar<Unit>(this->value.dot(otherVector));
        }
    };
}

/**
 * btVector3 UnitWrapper.
 * @tparam Unit Unit associated to this value.
 */
template<typename Unit>
using Vector3 = Units::UnitWrapper<btVector3,Unit>;

#endif /* VECTOR3_HPP */


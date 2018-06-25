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

#ifndef UNITS_TRANSFORM_HPP
#define UNITS_TRANSFORM_HPP

#include <type_traits>

#include "btBulletDynamicsCommon.h"

#include "units/BulletUnits.hpp"
#include "units/Vector3.hpp"

/** btTransform wrapper, with compile-time unit. */
template<typename Unit>
struct Transform {
    /** Data structure holding the numerical value of this object. */
    btTransform value;

    /**
     * Constructor forwarding directly to btTransform constructor.
     *
     * No conversion is performed by this constructor. The raw value constructed
     * from the constructor will directly be interpreted as Unit type.
     *
     * @param args Arguments of the raw value constructor.
     * @tparam ArgsTypes Types of the arguments.
     */
    template<typename... ArgTypes, typename std::enable_if_t<std::is_constructible<btTransform, ArgTypes...>::value,int> = 0>
    explicit Transform(ArgTypes&&... args) : value(std::forward<ArgTypes>(args)...) {

    }

    /**
     * Constructs a Transform as the composition of a rotation and a translation.
     * @param basis Rotation part of the transform.
     * @param origin Translation part of the transform.
     */
    Transform(const btMatrix3x3& basis, const Vector3<Unit>& origin) :
        value(basis, origin.value)
    {

    }

    /**
     * Copy constructor, with automatic unit conversion.
     * @param other Value to copy.
     * @tparam Unit of the copied value.
     */
    template<typename OtherUnit>
    Transform(const Transform<OtherUnit>& other) :
        Transform(other.getBasis(), other.getOrigin())
    {

    }

    /**
     * Gets the origin (translation vector) of this transform.
     * @return The origin of this transform.
     */
    Vector3<Unit> getOrigin() const {
        return Vector3<Unit>(value.getOrigin());
    }

    /**
     * Gets the basis (rotation matrix) of this transform.
     * @return The basis of this transform.
     */
    const btMatrix3x3& getBasis() const {
        return value.getBasis();
    }

    /**
     * Returns a reference to a Identity transform in this unit system.
     * @return A identity transform.
     */
    static const Transform& getIdentity() {
        static const Transform result(btTransform::getIdentity());
        return result;
    }
};

template<typename UnitL, typename UnitR, typename std::enable_if_t<UnitL() == UnitR(),int> =0>
Transform<UnitL> operator*(const Transform<UnitL>& l, const Transform<UnitR>& r) {
    return Transform<UnitL>(l.value * r.value);
}

/**
 * Converts the input value to a raw datatype in the Physic engine units.
 *
 * @param input Value to convert.
 * @return A raw transform, containing the value of the input in BulletUnits system.
 *
 * @tparam Unit Input's unit.
 */
template<typename Unit>
btTransform toBulletUnits(const Transform<Unit>& input) {
    return Transform<Units::UnitConverter<BulletUnits,Unit>>(input).value;
}

/**
 * Wraps a raw input value (physics engine units) in a UnitWrapper type.
 *
 * @param input Raw transform from the physics engine.
 * @return The input value converted to the desired unit.
 *
 * @tparam Unit Unit of the output transform. The caller must ensure that the input
 * raw transform has the same dimension, expressed in physics engine units.
 */
template<typename Unit>
Transform<Unit> fromBulletValue(const btTransform& input) {
    return Transform<Units::UnitConverter<BulletUnits,Unit>>(input);
}

#endif /* UNITS_TRANSFORM_HPP */

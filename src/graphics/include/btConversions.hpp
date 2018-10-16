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

#ifndef BTCONVERSIONS_HPP
#define BTCONVERSIONS_HPP

#include "btBulletDynamicsCommon.h"
#include "irrlicht.h"

/**
 * Converts a Bullet vector into an Irrlicht vector.
 * @param[in] vector Bullet vector.
 * @return Irrlicht vector.
 */
inline irr::core::vector3df btToIrrVector(const btVector3& vector) {
    return irr::core::vector3df(vector.x(), vector.y(), vector.z());
}

/**
 * Converts an Irrlicht vector into a Bullet vector.
 * @param[in] vector Irrlicht vector.
 * @return Bullet vector.
 */
inline btVector3 irrToBtVector(const irr::core::vector3df& vector) {
    return btVector3(vector.X, vector.Y, vector.Z);
}

/**
 * Converts a Bullet quaternion to a vector of Euler angles (Irrlicht format).
 * @param[in] btQuat Bullet quaternion.
 * @return Vector of Irrlicht Euler angles.
 */
inline irr::core::vector3df btQuaternionToEulerAngles(const btQuaternion& btQuat) {
    irr::core::quaternion irrQuat(btQuat.x(), btQuat.y(), btQuat.z(), btQuat.w());
    irr::core::vector3df result;
    irrQuat.toEuler(result);
    return irr::core::RADTODEG*result;
}

#endif /* BTCONVERSIONS_HPP */

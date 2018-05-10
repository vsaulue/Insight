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

#ifndef JOINT_HPP
#define JOINT_HPP

#include "btBulletDynamicsCommon.h"

#include <memory>

/** Common interface for all joint types. */
class Joint {
public:
    virtual ~Joint() = default;

    /**
     * Gets the Bullet constraint implementing this joint.
     * @return The constraint implementing this joint.
     */
    virtual std::shared_ptr<btTypedConstraint> getConstraint() = 0;
};

#endif /* JOINT_HPP */

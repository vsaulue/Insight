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

#ifndef SPHERICALJOINTFEEDBACKLOOP_HPP
#define SPHERICALJOINTFEEDBACKLOOP_HPP

#include "btBulletCollisionCommon.h"

#include "Action.hpp"
#include "FeedbackLoop.hpp"
#include "Sense.hpp"

/** Feedback loop for a SphericalJoint. */
class SphericalJointFeedbackLoop : public FeedbackLoop {
public:
    /**
     * SphericalJointFeedbackLoop constructor.
     * @param sense Input sense (relative rotation of the 2 bodies of the joint).
     * @param action output action (motor torque).
     */
    SphericalJointFeedbackLoop(const Sense<btQuaternion>& sense, Action<btVector3>& action);

    virtual ~SphericalJointFeedbackLoop();

    void stepSimulation() override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Relative orientation of the two parts of the joint. */
    const Sense<btQuaternion>& inputRotation;
    /** Torque exerced by the motor in the joint. */
    Action<btVector3>& outputMotorTorque;
    /** Target rotation between the two parts of the joint. */
    btQuaternion targetRotation;
    /** Rotation at the previous tick. */
    btQuaternion previousRotation;
};

#endif /* SPHERICALJOINTFEEDBACKLOOP_HPP */

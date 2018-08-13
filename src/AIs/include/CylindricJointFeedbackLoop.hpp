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

#ifndef CYLINDRICJOINTFEEDBACKLOOP_HPP
#define CYLINDRICJOINTFEEDBACKLOOP_HPP

#include "Action.hpp"
#include "FeedbackLoop.hpp"
#include "Sense.hpp"

/** Feedback loop for a CylindricJoint. */
class CylindricJointFeedbackLoop : public FeedbackLoop {
public:
    /**
     * CylindricJointFeedbackLoop constructor.
     * @param sense Input sense (relative rotation of the 2 bodies of the joint).
     * @param action output action (motor torque).
     */
    CylindricJointFeedbackLoop(const Sense<float>& sense, Action<float>& action);

    virtual ~CylindricJointFeedbackLoop();

    void stepSimulation() override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Relative orientation of the two parts of the joint. */
    const Sense<float>& inputRotation;
    /** Torque exerced by the motor in the joint. */
    Action<float>& outputMotorTorque;
    /** Target angle. */
    float targetAngle;
    /** Angle at the previous simulation step. */
    float previousAngle;
};

#endif /* CYLINDRICJOINTFEEDBACKLOOP_HPP */

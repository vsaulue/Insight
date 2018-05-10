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

#include "SphericalJoint.hpp"

static void initPosition(const CompoundBody& fixedBody, const btTransform& fixedJoint, CompoundBody& movingBody, const btTransform& movingJoint, const btQuaternion& rotation) {
    btTransform joint(rotation, btVector3(0,0,0));
    btTransform newTransform = fixedBody.getTransform() * fixedJoint * joint * movingJoint.inverse();
    movingBody.setTransform(newTransform);
}

SphericalJoint::SphericalJoint(CompoundBody& ball, CompoundBody& socket, const SphericalJointInfo& info) :
    ball(ball),
    socket(socket),
    jointInfo(info),
    constraint(std::make_shared<btConeTwistConstraint>(*ball.getBulletBody(), *socket.getBulletBody(), info.ballTransform, info.socketTransform))
{
    if (info.generateBallShape) {
        ball.addSphereD(info.jointDensity, info.ballRadius, info.ballTransform.getOrigin());
    }
    if (info.placeBall) {
        initPosition(socket, info.socketTransform, ball, info.ballTransform, info.startRotation);
    } else {
        initPosition(ball, info.ballTransform, socket, info.socketTransform, info.startRotation.inverse());
    }
}

SphericalJoint::~SphericalJoint() = default;

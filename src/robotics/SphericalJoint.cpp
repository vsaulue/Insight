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

static void initPosition(const Body& fixedBody, const Transform<SI::Length>& fixedJoint, Body& movingBody,
                         const Transform<SI::Length>& movingJoint, const btQuaternion& rotation)
{
    btTransform joint(rotation, btVector3(0,0,0));
    btTransform newTransform = fixedBody.getEngineTransform() * toBulletUnits(fixedJoint) * joint * toBulletUnits(movingJoint).inverse();
    movingBody.setEngineTransform(newTransform);
}

static std::shared_ptr<btConeTwistConstraint> makeConstraint(Body& ball, Body& socket, const SphericalJointInfo& info) {
    btTransform convexTr = toBulletUnits(info.convexTransform);
    btTransform concaveTr = toBulletUnits(info.concaveTransform);
    return std::make_shared<btConeTwistConstraint>(ball.getBulletBody(), socket.getBulletBody(), convexTr, concaveTr);
}

SphericalJoint::SphericalJoint(Body& ball, Body& socket, const SphericalJointInfo& info, bool placeBall) :
    Joint(ball, socket),
    jointInfo(info),
    constraint(makeConstraint(ball, socket, info))
{
    if (placeBall) {
        initPosition(socket, info.concaveTransform, ball, info.convexTransform, info.startRotation);
    } else {
        initPosition(ball, info.convexTransform, socket, info.concaveTransform, info.startRotation.inverse());
    }
}

SphericalJoint::~SphericalJoint() = default;

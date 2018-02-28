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

#include "Sphere.hpp"

static btVector3 calculateInertia(btCollisionShape* shape, btScalar mass) {
    btVector3 result;
    shape->calculateLocalInertia(mass, result);
    return result;
}

Sphere::Sphere(btScalar mass, btScalar radius) : Sphere(mass, std::make_unique<btSphereShape>(radius)) {

}

Sphere::Sphere(btScalar mass, std::unique_ptr<btSphereShape>&& shape) :
    Body(mass, *shape, calculateInertia(shape.get(), mass)),
    shape(std::move(shape))
{

}

void Sphere::drawShape(ShapeDrawer& drawer) const {
    static const btVector3 center = {0,0,0};
    drawer.drawSphere(center, shape->getRadius());
}

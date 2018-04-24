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

#include "CompoundBody.hpp"

class CompoundBody::Child {
public:
    /**
     * Creates a new child shape.
     *
     * @param[in] childIndex Index of this object in the Bullet compount shape.
     */
    Child(int childIndex) : index(childIndex) {

    }

    /**
     * Gets the Bullet collision shape of this object.
     * @return The collision shape of this object.
     */
    virtual btCollisionShape& getShape() = 0;

    /** Draw a 3d representation of this collision shape. */
    virtual void draw(ShapeDrawer& drawer, const btTransform& transform) = 0;

    virtual ~Child() = default;

    /**
     * Gets the index of this shape in the parent btCompoundBody.
     * @return
     */
    int getIndex() const {
        return index;
    };
private:
    /** Index of this shape in the parent btCompoundBody. */
    int index;
};

namespace CompoundBodyChildren {
    /** Sphere in a CompoundBody. */
    class Sphere : public CompoundBody::Child {
    public:
        /**
         * Creates a new sphere in a CompoundBody object.
         *
         * @param[in] childIndex Index of this object in hte btCompoundShape.
         * @param[in] radius Radius of the new sphere.
         */
        Sphere(int childIndex, btScalar radius) : Child(childIndex), shape(radius) {

        }

        btCollisionShape& getShape() override {
            return shape;
        }

        void draw(ShapeDrawer& drawer, const btTransform& transform) override {
            drawer.drawSphere(transform.getOrigin(), shape.getRadius());
        }
    private:
        /** Bullet shape. */
        btSphereShape shape;
    };

    /** Cylinder in a compound body. */
    class Cylinder : public CompoundBody::Child {
    public:
        /**
         * Creates a new cylinder in a CompoundBody object.
         * @param[in] childIndex Index of this object in the btCompoundShape.
         * @param[in] halfExtents Half extents of the new cylinder.
         */
        Cylinder(int childIndex, const btVector3& halfExtents) :
            Child(childIndex),
            shape(halfExtents)
        {

        }

        btCollisionShape& getShape() override {
            return shape;
        }

        void draw(ShapeDrawer& drawer, const btTransform& transform) override {
            drawer.drawCylinder(transform, shape.getHalfExtentsWithoutMargin());
        }
    private:
        /** Bullet shape. */
        btCylinderShape shape;
    };
}

CompoundBody::CompoundBody() : CompoundBody(std::make_unique<btCompoundShape>()) {

}

CompoundBody::CompoundBody(std::unique_ptr<btCompoundShape> shape) :
    Body(1234, *shape.get(), btVector3(1,1,1)),
    shape(std::move(shape))
{

}


CompoundBody::~CompoundBody() = default;

void CompoundBody::addSphere(btScalar radius, const btVector3& center) {
    using Sphere = CompoundBodyChildren::Sphere;
    std::unique_ptr<Sphere> newObject = std::make_unique<Sphere>(shape->getNumChildShapes(), radius);
    Sphere& ref = *newObject;
    children.insert(std::move(newObject));
    shape->addChildShape(btTransform(btQuaternion::getIdentity(), center), &ref.getShape());
}

void CompoundBody::addCylinder(const btTransform& transform, const btVector3& halfExtents) {
    using Cylinder = CompoundBodyChildren::Cylinder;
    std::unique_ptr<Cylinder> newObject = std::make_unique<Cylinder>(shape->getNumChildShapes(), halfExtents);
    Cylinder& ref = *newObject;
    children.insert(std::move(newObject));
    shape->addChildShape(transform, &ref.getShape());
}

void CompoundBody::drawShape(ShapeDrawer& drawer) const {
    for (const auto& child : children) {
        const btTransform& transform = shape->getChildTransform(child->getIndex());
        child->draw(drawer, transform);
    }
}

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

#ifndef CYLINDERSHAPE_HPP
#define CYLINDERSHAPE_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "Shape.hpp"

/**
 * Cylinder Shape.
 *
 * With a default transform, the cylinder has a central axis aligned with the Y axis. It is centered on {0,0,0}.
 */
class CylinderShape : public Shape {
public:
    /**
     * Creates a new cylinder shape.
     * @param mass Mass of this shape.
     * @param halfExtents Half extents (scale coefficients on each axis) of this new shape.
     */
    CylinderShape(btScalar mass, const btVector3& halfExtents);

    /**
     * Creates a new cylinder shape.
     * @param mass Density if this shape.
     * @param halfExtents Half extents (scale coefficients on each axis) of this new shape.
     */
    CylinderShape(Density density, const btVector3& halfExtents);

    virtual ~CylinderShape();

    /**
     * Gets the half extents of this cylinder.
     * @return The half extents of this cylinder.
     */
    btVector3 getHalfExtents() const;

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Creates a new CylinderShape from a Lua table.
     * @param table Lua table containing the parameters of the new shape.
     * @return The new shape.
     */
    static std::unique_ptr<CylinderShape> luaGetFromTable(LuaTable& table);
private:
    btCylinderShape shape;
};

#endif /* CYLINDERSHAPE_HPP */

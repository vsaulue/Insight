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

#ifndef STATICPLANESHAPE_HPP
#define STATICPLANESHAPE_HPP

#include <memory>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "Shape.hpp"

/** Static plane shape. */
class StaticPlaneShape : public Shape {
public:
    /** Creates a new static plane shape.
     * @param normal Normal vector of the plane.
     * @param offset Offset of the plane (the point origin+Offset*Normal belongs to the plane).
     */
    StaticPlaneShape(const btVector3& normal, btScalar offset);

    virtual ~StaticPlaneShape();

    /**
     * Gets a normal vector of the plane.
     * @return A normal vector of the plane.
     */
    const btVector3& getNormal() const;

    /**
     * Gets the offset parameter of the plane.
     *
     * The point Origin + getOffset()*getNormal is included in the plane.
     *
     * @return The offset parameter of the plane.
     */
    btScalar getOffset() const;

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Constructs a StaticPlaneShape from a Lua table.
     * @param table Lua table containing the parameters of the new shape.
     * @return The new shape.
     */
    static std::unique_ptr<StaticPlaneShape> luaGetFromTable(LuaTable& table);
private:
    /** Bullet shape. */
    btStaticPlaneShape shape;
};

#endif /* STATICPLANESHAPE_HPP */

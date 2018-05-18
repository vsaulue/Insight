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

#ifndef COMPOUNDSHAPE_HPP
#define COMPOUNDSHAPE_HPP

#include <memory>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "lua/types/LuaTable.hpp"
#include "Shape.hpp"

/** Shape made of multiple child shapes. */
class CompoundShape : public Shape {
public:
    /** Data to insert a child shape into a Compound shape. */
    struct ChildInfo {
        std::shared_ptr<Shape> shape; /** Shape to insert. */
        btTransform transform; /** Relative position & orientation of this child shape. */
    };

    /**
     * Constructs a new compound shape.
     *
     * <p>The center of mass of all shapes will be properly computed & used in the physics engine.</p>
     * <p>TODO: also rotate objects in order to be in the frame of principal axis</p>
     *
     * @param constructionInfo List of child & their transform.
     */
    CompoundShape(const std::vector<ChildInfo>& constructionInfo);

    virtual ~CompoundShape();

    btCollisionShape& getBulletShape() override;

    const btCollisionShape& getBulletShape() const override;

    void draw(ShapeDrawer& drawer, const btTransform& transform) const override;

    /**
     * Creates a new CompoundShape from a Lua table.
     *
     * @param table Lua table containing the shape parameters.
     * @return The new CompoundShape object.
     */
    static std::unique_ptr<CompoundShape> luaGetFromTable(LuaTable& table);
private:
    /** Bullet shape. */
    btCompoundShape shape;
    /** List of child shapes. */
    std::vector<std::shared_ptr<Shape>> children;
};

#endif /* COMPOUNDSHAPE_HPP */

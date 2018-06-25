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

#include "lua/bindings/bullet.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/luaVirtualClass/shared_ptr.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "CompoundShape.hpp"
#include "units/Scalar.hpp"

CompoundShape::CompoundShape(const std::vector<ChildInfo>& constructionInfo) {
    // TODO: compute center of mass & principal axis, and transform all children in order to be
    // in the center of inertia frame in the btCompoundShape.
    mass = Scalar<SI::Mass>(0);
    for (const ChildInfo& child : constructionInfo) {
        children.push_back(child.shape);
        shape.addChildShape(toBulletUnits(child.transform), &child.shape->getBulletShape());
        mass+= child.shape->getMass();
    }
}

CompoundShape::~CompoundShape() = default;

btCollisionShape& CompoundShape::getBulletShape() {
    return shape;
}

const btCollisionShape& CompoundShape::getBulletShape() const {
    return shape;
}

void CompoundShape::draw(ShapeDrawer& drawer, const btTransform& transform) const {
    for (unsigned i=0; i<children.size(); i++) {
        btTransform childAbsTransform = transform * shape.getChildTransform(i);
        children[i]->draw(drawer, childAbsTransform);
    }
}

std::unique_ptr<CompoundShape> CompoundShape::luaGetFromTable(LuaTable& table) {
    using Str = LuaNativeString;
    std::vector<ChildInfo> children;
    LuaTable childrenTable = table.get<Str, LuaTable>("children");
    for (int index = 1; childrenTable.has<float>(index); index++) {
        LuaTable childTable = childrenTable.get<float,LuaTable>(index);
        ChildInfo child = {
            childTable.get<Str, std::shared_ptr<Shape>>("shape"),
            childTable.get<Str, Transform<SI::Length>>("transform"),
        };
        children.push_back(child);
    }
    return std::make_unique<CompoundShape>(children);
}

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

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "btBulletDynamicsCommon.h"

#include "Body.hpp"

class Terrain : public Body {
public:
    Terrain();

    void drawShape(ShapeDrawer& drawer) const override;

private:
    Terrain(std::unique_ptr<btStaticPlaneShape>&& shape);

    std::unique_ptr<btStaticPlaneShape> shape;
};

#endif /* TERRAIN_HPP */

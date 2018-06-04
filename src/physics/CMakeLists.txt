# This file is part of Insight.
# Copyright (C) 2017-2018 Vincent Saulue-Laborde <vincent_saulue@hotmail.fr>
#
# Insight is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Insight is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Insight.  If not, see <http://www.gnu.org/licenses/>.

add_library(PhysicEngine STATIC
    Body.cpp
    bullet.cpp
    CompoundShape.cpp
    ConvexHullShape.cpp
    CuboidShape.cpp
    CylinderShape.cpp
    Shape.cpp
    SphereShape.cpp
    StaticPlaneShape.cpp
    World.cpp
)

target_include_directories(PhysicEngine PUBLIC include)
target_include_directories(PhysicEngine PUBLIC ${BULLET_INCLUDE_DIRS})
target_link_libraries(PhysicEngine
    ${BULLET_LIBRARIES}
    LuaWrapper
)
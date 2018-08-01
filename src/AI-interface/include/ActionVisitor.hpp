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

#ifndef ACTIONVISITOR_HPP
#define ACTIONVISITOR_HPP

#include "btBulletDynamicsCommon.h"

#include "Action.hpp"

/** Abstract class for algorithms working on ActionSignal objects. */
class ActionVisitor {
public:
    /**
     * Apply the algorithm to a single float action signal.
     *
     * @param sense Object on which to apply the algorithm.
     */
    virtual void visit(Action<float>& action) = 0;

    /**
     * Apply the algorithm to a btVector3 action signal.
     *
     * @param sense Object on which to apply the algorithm.
     */
    virtual void visit(Action<btVector3>& action) = 0;
};

#endif /* ACTIONVISITOR_HPP */

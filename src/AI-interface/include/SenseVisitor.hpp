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

#ifndef SENSEVISITOR_HPP
#define SENSEVISITOR_HPP

#include "Sense.hpp"

/** Abstract class for algorithms working on SenseSignal objects. */
class SenseVisitor {
public:
    /**
     * Apply the algorithm to a single float sense signal.
     *
     * @param sense Object on which to apply the algorithm.
     */
    virtual void visit(const Sense<float>& sense) = 0;

    /**
     * Apply the algorithm to a quaternion sense signal.
     *
     * @param sense Object on which to apply the algorithm.
     */
    virtual void visit(const Sense<btQuaternion>& sense) = 0;
};

#endif /* SENSEVISITOR_HPP */

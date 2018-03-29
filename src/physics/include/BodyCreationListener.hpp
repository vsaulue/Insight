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

#ifndef BODYCREATIONLISTENER_HPP
#define BODYCREATIONLISTENER_HPP

class BodyCreationListener;

#include "Body.hpp"
#include "World.hpp"

/** Interface to be informed of bodies creation by the physics engine. */
class BodyCreationListener {
public:
    /**
     * Event triggered when a new Body was added in the physics engine.
     *
     * @param[in] newBody The new body.
     */
    virtual void onBodyCreation(const Body& newBody) = 0;

    virtual ~BodyCreationListener() = default;
};

#endif /* BODYCREATIONLISTENER_HPP */

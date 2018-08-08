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

#ifndef AIFACTORY_HPP
#define AIFACTORY_HPP

#include <functional>
#include <memory>

#include "AI.hpp"
#include "AIInterface.hpp"

/** Class used to create new instances of an AI. */
class AIFactory {
public:
    /**
     * AIFactory constructor.
     * @param constructor Function used to create a new AI.
     */
    AIFactory(std::function<std::unique_ptr<AI>(AIInterface&)> constructor) :
        constructor(constructor)
    {

    }

    /**
     * Creates a new AI.
     * @param interface Interface to the body controlled by this AI.
     * @return The new AI.
     */
    std::unique_ptr<AI> createAI(AIInterface& interface) const {
        return constructor(interface);
    }
private:
    /** Function to call to create a new AI. */
    std::function<std::unique_ptr<AI>(AIInterface&)> constructor;
};

#endif /* AIFACTORY_HPP */

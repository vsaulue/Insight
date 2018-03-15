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

#ifndef INPUTEVENT_HPP
#define INPUTEVENT_HPP

#include "irrlicht.h"

#include "Keyboard.hpp"

/** Generic class to describe any input from the user. */
class InputEvent {
public:
    virtual ~InputEvent() = default;

    /**
     * Tells if this event is occuring on the given devices.
     *
     * @param[in] keyboard Dvice on which the event is tested.
     * @return True if the event occured on the given devices.
     */
    virtual bool happened(const Keyboard& keyboard) = 0;
};

#endif /* INPUTEVENT_HPP */

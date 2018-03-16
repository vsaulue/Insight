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

#ifndef MOUSE_HPP
#define MOUSE_HPP

#include "irrlicht.h"

/** Object handling mouse inputs. */
class Mouse {
public:
    /**
     * Creates a new mouse input handler.
     *
     * @param cursor Irrlicht handle of the mouse cursor.
     */
    Mouse(irr::gui::ICursorControl& cursor) : cursor(cursor) {

    }
private:
    /** Irrlicht cursor handle. */
    irr::gui::ICursorControl& cursor;
};

#endif /* MOUSE_HPP */

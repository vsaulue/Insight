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
    Mouse(irr::gui::ICursorControl& cursor) : cursor(cursor), mouseLocked(false) {

    }

    /***
     * Tells this mouse handler that a new frame will be rendered.
     */
    void newFrame();

    /** Switch the state of the mouse between locked & unlocked. */
    void toggleMouseLock() {
        setMouseLock(!mouseLocked);
    }

    /**
     * Locks or unlocks the mouse in the middle of the window.
     * @param locked True to lock the mouse, false to unlock.
     */
    void setMouseLock(bool locked) {
        mouseLocked = locked;
        cursor.setVisible(!mouseLocked);
    }

    /**
     * Tests if the mouse is locked in the middle of the window.
     * @return True if the mouse is locked.
     */
    bool isMouseLocked() const {
        return mouseLocked;
    }
private:
    /** Irrlicht cursor handle. */
    irr::gui::ICursorControl& cursor;

    /** Flag sets when the mouse cursor is locked in the middle of the screen & invisible. */
    bool mouseLocked;
};

#endif /* MOUSE_HPP */

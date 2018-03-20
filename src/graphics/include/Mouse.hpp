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
     * @param driver Irrlicht video driver.
     */
    Mouse(irr::gui::ICursorControl& cursor, const irr::video::IVideoDriver& driver) :
        cursor(cursor),
        driver(driver),
        mouseLocked(false),
        preEventPos(cursor.getPosition()),
        postEventPos(preEventPos)
    {

    }

    /***
     * Tells this mouse handler that a new frame will be rendered.
     */
    void newFrame();

    /**
     * Process a specific event coming form the GUI.
     *
     * @param[in] event Event to process.
     */
    void OnEvent(const irr::SEvent::SMouseInput& event);

    /**
     * Gets the movement done by the cursor since the last frame.
     * @return The displacement (in pixels) of the cursor sinte the last frame.
     */
    irr::core::position2d<irr::s32> getMove() const {
        return postEventPos-preEventPos;
    }

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
        if (locked) {
            cursor.setPosition(0.5f, 0.5f);
        }
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

    /** Irrlicht video driver. */
    const irr::video::IVideoDriver& driver;

    /** Flag sets when the mouse cursor is locked in the middle of the screen & invisible. */
    bool mouseLocked;

    /** Position of the cursor before processing the events that happened since the last frame was rendered? */
    irr::core::position2d<irr::s32> preEventPos;
    /** Current position of the cursor. */
    irr::core::position2d<irr::s32> postEventPos;
};

#endif /* MOUSE_HPP */

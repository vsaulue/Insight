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

#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <unordered_map>

#include "irrlicht.h"

/** Object handling keyboard inputs. */
class Keyboard {
public:
    /**
     * Tells if a specific kay is currently pressed by the user.
     *
     * @param[in] key Keyboard key to check.
     * @return True if the specified key is held down.
     */
    bool isKeyDown(irr::EKEY_CODE key) const;

    /**
     * Tells if a specific key was just pressed by the user.
     *
     * A key was "just pressed", if the key was not pressed while rendering the previous
     * frame, but is currently pressed.
     *
     * @param[in] key Keyboard key to check.
     * @return True if the key just was just pressed.
     */
    bool isKeyMovingDown(irr::EKEY_CODE key) const;

    /**
     * Tells if a specific key was just released by the user.
     *
     * A key was "just released", if the key was pressed while rendering the previous
     * frame, but is not pressed anymore.
     *
     * @param[in] key Keyboard key to check.
     * @return True if the key was just released.
     */
    bool isKeyMovingUp(irr::EKEY_CODE key) const;

    /**
     * Process a specific event coming form the GUI.
     *
     * @param[in] event Event to process.
     */
    void OnEvent(const irr::SEvent::SKeyInput& event);

    /**
     * Resets "moving" key list.
     */
    void newFrame();
private:
    /** Map indicating if a given key is pressed. */
    std::unordered_map<irr::EKEY_CODE, bool> keyDown;
    /** Map indicating if a given key state has just changed (up -> down, or down -> up). */
    std::unordered_map<irr::EKEY_CODE, bool> keyMoving;
};

#endif /* KEYBOARD_HPP */

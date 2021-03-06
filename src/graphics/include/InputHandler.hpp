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

#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "irrlicht.h"

#include "Action.hpp"
#include "Bindings.hpp"
#include "EventReport.hpp"
#include "InputSettings.hpp"
#include "Keyboard.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "Mouse.hpp"

/**
 * Handles user inputs in the GUI.
 */
class InputHandler : public irr::IEventReceiver, public LuaVirtualClass {
public:
    /**
     * Check if the key associated to a given action is pressed.
     *
     * @param[in] action Action to check.
     * @return True if the button associated to the action is pressed.
     */
    bool happened(Action action) const {
        InputEvent& event = bindings.getEvent(action);
        return event.happened(keyboard, mouse);
    };

    /**
     * Tests if the input event associated to a given action happened.
     *
     * @param[in] action Action to check.
     * @return Data about the tested action (did it happen, intensity).
     */
    EventReport actionReport(Action action) const {
        InputEvent& event = bindings.getEvent(action);
        return event.eventReport(keyboard, mouse);
    }

    bool OnEvent(const irr::SEvent& event) override;

    /**
     * Tells the input handlers that a new frame will be rendered.
     *
     * This function is used to reset all temporary status from the different inputs
     * (example: key passed from "up" to "down" in the last frame).
     */
    void newFrame();

    /**
     * Process actions reletad to input devices.
     */
    void doActions();

    /**
     * Creates a new input handler.
     *
     * @param device Irrlicht device.
     */
    InputHandler(irr::IrrlichtDevice& device) :
        device(device),
        mouse(*device.getCursorControl(), *device.getVideoDriver())
    {

    }

    /**
     * Gets the input settings.
     *
     * @return An object holding all user inputs settings.
     */
    const InputSettings& getSettings() const {
        return settings;
    }

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Irrlicht device. */
    irr::IrrlichtDevice& device;
    /** Object mapping keyboard & mouse button to GUI actions. */
    Bindings bindings;
    /** Keyboard handler. */
    Keyboard keyboard;
    /** Mouse handler. */
    Mouse mouse;
    /** Input settings. */
    InputSettings settings;
};

#endif /* INPUTHANDLER_HPP */


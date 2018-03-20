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

#include "InputHandler.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"

bool InputHandler::OnEvent(const irr::SEvent& event) {
    if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        keyboard.OnEvent(event.KeyInput);
    } else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        mouse.OnEvent(event.MouseInput);
    }
    return false;
}

void InputHandler::newFrame() {
    keyboard.newFrame();
    mouse.newFrame();
}

void InputHandler::doActions() {
    if (!device.isWindowFocused()) {
        mouse.setMouseLock(false);
    } else if (happened(Action::ToggleMouseLock)) {
        mouse.toggleMouseLock();
    }
}

int InputHandler::luaIndex(const std::string& memberName, LuaStateView& state) {
    if (memberName == "bindings") {
        state.push<Bindings*>(&bindings);
        return 1;
    }
    return 0;
}

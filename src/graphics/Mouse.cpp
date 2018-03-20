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

#include "Mouse.hpp"

void Mouse::newFrame() {
    if (mouseLocked) {
        irr::core::dimension2d<irr::u32> screenSize = driver.getScreenSize();
        preEventPos.X = screenSize.Width/2;
        preEventPos.Y = screenSize.Height/2;
        postEventPos = cursor.getPosition();
        cursor.setPosition(0.5f, 0.5f);
    } else {
        preEventPos = postEventPos;
        postEventPos = cursor.getPosition();
    }
}

void Mouse::OnEvent(const irr::SEvent::SMouseInput& event) {

}

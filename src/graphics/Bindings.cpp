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

#include "Bindings.hpp"

Bindings::Bindings() {
    actionToKey[Action::CameraForward] = irr::KEY_KEY_W;
    actionToKey[Action::CameraBackward] = irr::KEY_KEY_S;
    actionToKey[Action::CameraLeft] = irr::KEY_KEY_A;
    actionToKey[Action::CameraRight] = irr::KEY_KEY_D;
    actionToKey[Action::CameraUp] = irr::KEY_LSHIFT;
    actionToKey[Action::CameraDown] = irr::KEY_LCONTROL;

    buildKeyToAction();
}

void Bindings::buildKeyToAction() {
    for (const auto& [action, key] : actionToKey) {
        keyToAction[key] = action;
    }
}

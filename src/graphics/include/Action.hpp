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

#ifndef ACTION_HPP
#define ACTION_HPP

/** List of actions from the user in the GUI*/
enum class Action {
    CameraForward, /**< Move the camera forward. */
    CameraBackward, /**< Move the camera backward. */
    CameraLeft, /**< Move the camera to its left. */
    CameraRight, /**< Move the camera to its right. */
    CameraUp, /**< Move the camera upward. */
    CameraDown,  /**< Move the camera downward. */
    ToggleMouseLock, /**< Lock/unlock the mouse in the middle of the screen. */
    CameraTurnLeft, /**< Turn the camera to look on the left. */
    CameraTurnRight, /**< Turn the camera to look on the right. */
    CameraTurnUp, /**< Turn the camera to look upwards. */
    CameraTurnDown /**< Turn the camera to look downwards. */
};

#endif /* ACTION_HPP */


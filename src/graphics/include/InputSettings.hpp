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

#ifndef INPUTSETTINGS_HPP
#define INPUTSETTINGS_HPP

#include <map>
#include <unordered_map>

#include "lua/types/LuaVirtualClass.hpp"
#include "units/Floats.hpp"
#include "units/SI.hpp"

/** Class holdings all settings related to user inputs in the GUI. */
class InputSettings : public LuaVirtualClass {
public:
    /** Creates a new InputSettings with default values. */
    InputSettings();

    /** Camera rotation speed around each axis. */
    Units::Float<SI::AngularVelocity> cameraRotationSpeed;
    /** Camera translation speed on each axis. */
    Units::Float<SI::Speed> cameraTranslationSpeed;

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
};

#endif /* INPUTSETTINGS_HPP */

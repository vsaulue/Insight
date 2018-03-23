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

/** Class holdings all settings related to user inputs in the GUI. */
class InputSettings : public LuaVirtualClass {
public:
    /** Set of possible settings. */
    enum class SettingsType {
        CameraTranslationSpeed, /**< Camera translation speed on each axis. */
        CameraRotationSpeed, /**< Camera rotation speed around each axis. */
    };

    /** Creates a new InputSettings with default values. */
    InputSettings();

    /**
     * Gets the value of a settings.
     *
     * @param settings Settings type.
     * @return The value of settings.
     */
    float get(SettingsType settings) const;

    /**
     * Gets a map of all settings, and their names.
     *
     * @return A map of settings name, indexed by their enum value.
     */
    static const std::map<SettingsType, std::string>& list();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;
private:
    /** Map of all settings values indexed by their type. */
    std::unordered_map<SettingsType, float> values;
};

#endif /* INPUTSETTINGS_HPP */

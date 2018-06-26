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

#include <algorithm>
#include <map>
#include <memory>
#include <sstream>

#include "InputSettings.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/bindings/units/Floats.hpp"
#include "lua/LuaException.hpp"
#include "lua/types/LuaMethod.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "units/Floats.hpp"
#include "units/framework/UnitSymbols.hpp"

/**
 * Object used to interact with settings from Lua.
 *
 * This class is used as a type-eraser for the unit attached to the settings.
 */
class Setting {
private:
    /** Wraps a Float<Unit> and provides abstraction from the Unit. */
    class UnitEraser {
    public:
        virtual ~UnitEraser() = default;

        /**
         * Pushes the underlying Float<...> on a Lua stack.
         * @param state Stack on which the Float should be pushed.
         */
        virtual void luaPush(LuaStateView& state) = 0;

        /**
         * Set the underlying Float<...>, froma value read on a Lua stack.
         * @param state Stack in which the value is read.
         * @param stackIndex Index of the input in the stack.
         */
        virtual void luaGet(LuaStateView& state, int stackIndex) = 0;

        /**
         * Gets the unit symbol of the underlying Float<...>.
         * @return The unit symbol of the underlying value.
         */
        virtual const std::string& getUnitSymbol() = 0;

        /**
         * Gets a string representing the underlying Float<...> value.
         * @return A text representation of the wrapped value.
         */
        virtual std::string luaToString() = 0;
    };

    /**
     * Implementation of UnitEraser.
     * @tparam Unit Unit of the wrapped Float<...> type.
     */
    template<typename Unit>
    class UnitEraserImpl : public UnitEraser {
    public:
        /**
         * Wraps a Float<Unit> in a UnitEraser object.
         * @param wrappedValue Value to wrap by reference (not by copy).
         */
        UnitEraserImpl(Units::Float<Unit>& wrappedValue) : value(wrappedValue) {

        }

        void luaPush(LuaStateView& state) override;

        void luaGet(LuaStateView& state, int stackIndex) override;

        const std::string& getUnitSymbol() override;

        std::string luaToString() override;
    private:
        /** Reference to the wrapped value.*/
        Units::Float<Unit>& value;
    };
public:
    /**
     * Constructs a new setting element.
     * @param wrappedValue Reference to the value storing the setting data.
     */
    template<typename Unit>
    Setting(Units::Float<Unit>& wrappedValue) :
        impl(std::make_unique<UnitEraserImpl<Unit>>(wrappedValue))
    {

    }

    /**
     * Gets the virtual object wrapping the setting value.
     * @return The UnitEraser of this object.
     */
    UnitEraser& getImpl() {
        return *impl;
    }
private:
    /** Virtual object abstracting the unit of the underlying Float<...>. */
    std::unique_ptr<UnitEraser> impl;
};

template<typename Unit>
void Setting::UnitEraserImpl<Unit>::luaPush(LuaStateView& state) {
    state.push<Units::Float<Unit>>(value);
}

template<typename Unit>
void Setting::UnitEraserImpl<Unit>::luaGet(LuaStateView& state, int stackIndex) {
    value = state.get<Units::Float<Unit>>(stackIndex);
}

template<typename Unit>
const std::string& Setting::UnitEraserImpl<Unit>::getUnitSymbol() {
    return Units::unitSymbol<Unit>();
}

template<typename Unit>
std::string Setting::UnitEraserImpl<Unit>::luaToString() {
    std::ostringstream result;
    result << "{value=" << value.value << ", unit='" << Units::unitSymbol<Unit>() << "'}";
    return result.str();
}

template<>
class LuaBinding<Setting> : public LuaDefaultBinding<Setting> {
public:
    static int luaIndexImpl(Setting& object, const std::string& memberName, LuaStateView& state) {
        using Method = LuaMethod<Setting>;
        int result = 1;
        if (memberName == "value") {
            object.getImpl().luaPush(state);
        } else if (memberName == "unit") {
            state.push<LuaNativeString>(object.getImpl().getUnitSymbol().c_str());
        } else if (memberName == "setValue") {
            state.push<Method>([](Setting& object, LuaStateView& state) -> int {
                object.getImpl().luaGet(state,2);
                return 0;
            });
        } else {
            result = 0;
        }
        return result;
    }

    static std::string luaToStringImpl(Setting& object) {
        return object.getImpl().luaToString();
    }
};

InputSettings::InputSettings() {
    cameraTranslationSpeed = Units::Float<SI::Speed>(5);
    cameraRotationSpeed = Units::Float<SI::AngularVelocity>(0.45);
}

/**
 * Creates a Setting object wrapping a member of InputSetting.
 * @param settings Object whose field will be wrapped.
 * @return A Setting object wrapping a member of settings.
 * @tparam member InputSettings member to be wrapped by this function.
 */
template<auto InputSettings::*member>
Setting settingCreator(InputSettings& settings) {
    return Setting(settings.*member);
}

/**
 * Map: setting name -> setting creator function.
 *
 * The keys are the name of each setting as accessible from Lua. The values are
 * function that can be used to wrap the associated member of an InputSetting object.
 */
static const std::map<std::string,Setting(*)(InputSettings&)> nameToSetting = {
        {"CameraTranslationSpeed", settingCreator<&InputSettings::cameraTranslationSpeed>},
        {"CameraRotationSpeed", settingCreator<&InputSettings::cameraRotationSpeed>},
};

int InputSettings::luaIndex(const std::string& memberName, LuaStateView& state) {
    using Method = LuaMethod<InputSettings>;
    int result = 1;
    if (memberName == "list") {
        state.push<Method>([](InputSettings& obj, LuaStateView& state) -> int {
            int result = nameToSetting.size();
            state.checkStack(result);
            for (auto& pair : nameToSetting) {
                state.push<LuaNativeString>(pair.first.c_str());
            }
            return result;
        });
    } else {
        auto it = nameToSetting.find(memberName);
        if (it != nameToSetting.end()) {
            state.push<Setting>(it->second(*this));
        } else {
            result = 0;
        }
    }
    return result;
}

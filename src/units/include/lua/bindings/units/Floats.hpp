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

#ifndef LUA_BINDINGS_UNITS_FLOATS_HPP
#define LUA_BINDINGS_UNITS_FLOATS_HPP

#include <sstream>
#include <string>

#include "lua/bindings/LuaDefaultBinding.hpp"
#include "lua/LuaBinding.hpp"
#include "units/Floats.hpp"
#include "units/framework/UnitSymbols.hpp"

namespace Units {
    namespace details {
        /**
         * Type used to get the Lua class name of floating point wrappers.
         * @tparam Floating point type.
         */
        template<typename FloatType>
        struct LuaFloatName;

        template<>
        struct LuaFloatName<float> {
            static constexpr char value[] = "Float";
        };

        template<>
        struct LuaFloatName<double> {
            static constexpr char value[] = "Double";
        };
    }
}

template<typename FloatType, typename Unit>
class LuaBinding<Units::UnitWrapper<FloatType,Unit>,typename std::enable_if<std::is_floating_point<FloatType>::value>::type> 
    : public LuaDefaultBinding<Units::UnitWrapper<FloatType,Unit>>
{
private:
    using BoundType = Units::UnitWrapper<FloatType,Unit>;
    using LuaName = Units::details::LuaFloatName<FloatType>;
    using Base = LuaDefaultBinding<Units::UnitWrapper<FloatType,Unit>>;
public:
    static const std::string& luaClassName() {
        static const std::string className(std::string(LuaName::value) + "<" + Units::unitSymbol<Unit>() + ">");
        return className;
    }

    static BoundType get(LuaStateView& state, int stackIndex) {
        if (state.isNumber(stackIndex)) {
            return BoundType(state.get<FloatType>(stackIndex));
        } else {
            return Base::get(state, stackIndex);
        }
    }

    static int luaIndexImpl(BoundType& object, const std::string& memberName, LuaStateView& state) {
        int result = 1;
        if (memberName == "value") {
            state.push<FloatType>(object.value);
        } else {
            result = 0;
        }
        return result;
    }

    static std::string luaToStringImpl(BoundType& object) {
        std::ostringstream result;
        result << "{value=" << object.value << "}";
        return result.str();
    }
};


#endif /* LUA_BINDINGS_UNITS_FLOATS_HPP */

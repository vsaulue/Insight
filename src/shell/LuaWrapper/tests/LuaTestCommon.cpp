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

#include <functional>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "LuaTestCommon.hpp"

#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/LuaStateView.hpp"
#include "lua/types/LuaFunction.hpp"

void defineReadBool(LuaStateView& state, bool& res) {
    std::function<int(LuaStateView&)> function = [&res](LuaStateView& state) -> int {
        res = state.get<bool>(1);
        return 0;
    };
    state.push<LuaFunction>(function);
    state.setGlobal("readBool");
}

void defineReadFloat(LuaStateView& state, float& res) {
    std::function<int(LuaStateView&)> function = [&res](LuaStateView& state) -> int {
        res = state.get<float>(1);
        return 0;
    };
    state.push<LuaFunction>(function);
    state.setGlobal("readFloat");
}

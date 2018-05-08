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

#include <cstring>

#include <catch.hpp>

#include "lua/LuaState.hpp"
#include "lua/bindings/FundamentalTypes.hpp"
#include "lua/types/LuaFunction.hpp"
#include "lua/types/LuaNativeString.hpp"
#include "lua/types/LuaTable.hpp"

TEST_CASE("Lua table bindings.") {
    LuaState state;
    SECTION("push<LuaTable>()") {
        state.push<LuaTable>();
        state.pushValue(-1);
        state.setGlobal("globalTable");
        state.doString("globalTable.num=42");

        // helper function: string reader
        const char *readStr = nullptr;
        state.push<LuaFunction>([&readStr](LuaStateView& state) -> int {
            readStr = state.get<LuaNativeString>(1);
            return 0;
        });
        state.setGlobal("readStr");
        // helper function: float reader.
        float readFloat = 0;
        state.push<LuaFunction>([&readFloat](LuaStateView& state) -> int {
            readFloat = state.get<float>(1);
            return 0;
        });
        state.setGlobal("readFloat");

        SECTION("get<LuaTable>()") {
            LuaTable table = state.get<LuaTable>(1);
            SECTION("LuaTable.get()") {
                float num = table.get<LuaNativeString, float>("num");
                REQUIRE(num==42);
            }

            SECTION("LuaTable.set()") {
                const char *str = "OmeletteDuFromage";
                float key = -2;
                table.set<float, LuaNativeString>(key, str);

                state.doString("readStr(globalTable[-2])");
                REQUIRE(std::strcmp(readStr, str) == 0);
            }

            SECTION("Create subTable.") {
                float subValue = -29;
                std::string subFieldName("subField");
                state.push<LuaTable>();
                LuaTable subTable = state.get<LuaTable>(-1);
                subTable.set<LuaNativeString, float>(subFieldName.c_str(), subValue);

                SECTION("globalTable[float]=subTable (C++)") {
                    float tableKey = -12.5f;
                    table.set<float, LuaTable>(tableKey, subTable);

                    state.doString(std::string("readFloat(globalTable[-12.5].") + subFieldName + ")");
                    REQUIRE(readFloat==subValue);
                }

                SECTION("globalTable[subTable]=float (C++)") {
                    std::string value = "Kilroy was here.";
                    table.set<LuaTable, LuaNativeString>(subTable, value.c_str());
                    state.push<LuaTable>(subTable);
                    state.setGlobal("subTable");

                    state.doString(std::string("readStr(globalTable[subTable])"));
                    REQUIRE(value == readStr);
                }
            }

            SECTION("globalTable[float]=subTable (Lua)") {
                state.doString("globalTable.someField={foo=\"bar\"}");
                SECTION("LuaTable.get<float,LuaTable>(...)") {
                    LuaTable subTable = table.get<LuaNativeString, LuaTable>("someField");
                    const char *str = subTable.get<LuaNativeString, LuaNativeString>("foo");
                    REQUIRE(std::strcmp(str,"bar")==0);
                }
            }
        }
    }

    /* Check that automatic cleanup of the stack works. */
    REQUIRE(state.getTop() == 0);
}

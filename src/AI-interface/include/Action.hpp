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

#include <functional>

#include "btBulletCollisionCommon.h"

#include "ActionSignal.hpp"
#include "lua/bindings/luaVirtualClass/base.hpp"
#include "lua/types/LuaMethod.hpp"

/**
 * Action signal wrapping a specific type of data.
 * @tparam T Type of data handled by this action signal.
 */
template<typename T>
class Action;

namespace details {
    /**
     * Default implementation of some methods of Action<T>.
     * @tparam T Type of data handled by this action signal.
     */
    template<typename T>
    class DefaultAction : public ActionSignal {
    public:
        /**
         * DefaultAction constructor.
         * @param getter Function used to set the value of this action signal.
         */
        DefaultAction(std::function<void(const T&)>&& setter) : setter(setter) {

        }

        /**
         * Sets the value of this action signal.
         * @param value The new value.
         */
        void set(const T& value) const {
            setter(value);
        };

        int luaIndex(const std::string& memberName, LuaStateView& state) override {
            using Method = LuaMethod<DefaultAction>;
            int result = 1;
            if (memberName=="set") {
                state.push<Method>([](DefaultAction& object, LuaStateView& state) -> int {
                    object.setter(state.get<T>(2));
                    return 0;
                });
            } else {
                result = 0;
            }
            return result;
        }
    private:
        /** Function used to set the value of this signal. */
        std::function<void(const T&)> setter;
    };
}

template<>
class Action<float> : public details::DefaultAction<float> {
public:
    using DefaultAction::DefaultAction;

    void apply(ActionVisitor& visitor) override;
};

template<>
class Action<btVector3> : public details::DefaultAction<btVector3> {
public:
    using DefaultAction::DefaultAction;

    void apply(ActionVisitor& visitor) override;
};

#endif /* ACTION_HPP */

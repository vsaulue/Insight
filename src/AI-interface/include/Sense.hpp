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

#ifndef SENSE_HPP
#define SENSE_HPP

#include <functional>

#include "btBulletCollisionCommon.h"

#include "lua/bindings/bullet.hpp"
#include "SenseSignal.hpp"

template<typename T>
class Sense;

namespace details {
    template<typename T>
    class DefaultSense : public SenseSignal {
    public:
        DefaultSense(std::function<T()>&& getter) : getter(getter) {

        }

        T get() const {
            return getter();
        };

        int luaIndex(const std::string& memberName, LuaStateView& state) override {
            int result = 1;
            if (memberName=="value") {
                state.push<T>(getter());
            } else {
                result = 0;
            }
            return result;
        }
    private:
        std::function<T()> getter;
    };
}

template<>
class Sense<float> : public details::DefaultSense<float> {
public:
    using DefaultSense::DefaultSense;

    void apply(SenseVisitor& visitor) const override;
};

template<>
class Sense<btQuaternion> : public details::DefaultSense<btQuaternion> {
public:
    using DefaultSense::DefaultSense;

    void apply(SenseVisitor& visitor) const override;
};

#endif /* SENSE_HPP */

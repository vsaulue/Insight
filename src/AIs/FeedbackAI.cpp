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

#include <stdexcept>

#include "CylindricJointFeedbackLoop.hpp"
#include "FeedbackAI.hpp"
#include "FeedbackLoop.hpp"
#include "lua/bindings/luaVirtualClass/pointers.hpp"
#include "SenseVisitor.hpp"
#include "SphericalJointFeedbackLoop.hpp"

/** Helper class building a FeedbackLoop objects from a sense. */
class FeedbackLoopConstructor : public SenseVisitor {
public:
    /**
     * FeedbackLoopConstructor constructor.
     *
     * This does NOT create the feedback loop. A call to SenseSignal.apply(thisObject);
     * is needed.
     *
     * @param senseName Name of the sense.
     * @param actions Set of actions, containing the action matching the given sense name.
     */
    FeedbackLoopConstructor(const std::string& senseName, const std::unordered_map<std::string,ActionSignal*> actions) :
        jointName(getJointName(senseName)),
        feedbackLoop(nullptr)
    {
        actionSignal = nullptr;
        auto it = actions.find(jointName + ".motor");
        if (it != actions.end()) {
            actionSignal = it->second;
        }
    }

    void visit(const Sense<btQuaternion>& sense) override {
        auto action = dynamic_cast<Action<btVector3>*>(actionSignal);
        if (action != nullptr) {
            feedbackLoop = std::make_unique<SphericalJointFeedbackLoop>(sense, *action);
        }
    }

    void visit(const Sense<float>& sense) override {
        auto action = dynamic_cast<Action<float>*>(actionSignal);
        if (action != nullptr) {
            feedbackLoop = std::make_unique<CylindricJointFeedbackLoop>(sense, *action);
        }
    }

    /**
     * Gets the name of the joint computed from the sense name.
     * @return The name of the joint controlled by the constructed feedback loop.
     */
    const std::string& getJointName() const {
        return jointName;
    }

    /**
     * Gets the constructed FeedbackLoop object and takes ownership.
     * @return The constructed FeedbackLoop object.
     */
    std::unique_ptr<FeedbackLoop> getFeedbackLoop() {
        return std::move(feedbackLoop);
    }
private:
    /** Name of the joint controlled by the new FeedbackLoop. */
    std::string jointName;
    /** Action signal controlled by the new FeedbackLoop. */
    ActionSignal* actionSignal;
    /** The new FeedbackLoop created by this object. */
    std::unique_ptr<FeedbackLoop> feedbackLoop;

    /**
     * Gets the name of the motor controlled by the new FeedbackLoop.
     * @return The name of the motor.
     */
    std::string getMotorName() {
        return jointName + ".motor";
    }

    /**
     * Gets the name of a joint from the name of a sense signal.
     * @param senseName The name of the SenseSignal of a joint (proprioception sense).
     * @return The name of the joint associated to this sense.
     */
    static std::string getJointName(const std::string& senseName) {
        auto index = senseName.find_last_of('.');
        return senseName.substr(0, index);
    }
};

FeedbackAI::FeedbackAI(AIInterface& interface) : AI(interface) {
    for (auto pair : interface.getSenses()) {
        FeedbackLoopConstructor constructor(pair.first, interface.getActions());
        pair.second->apply(constructor);
        auto newLoop = constructor.getFeedbackLoop();
        if (newLoop.get() != nullptr) {
            loops[constructor.getJointName()] = std::move(newLoop);
        } else {
            std::string msg = std::string("Unable to build a feedback loop for sense:") + pair.first;
            throw std::invalid_argument(msg);
        }
    }
}

FeedbackAI::~FeedbackAI() = default;

int FeedbackAI::luaIndex(const std::string& memberName, LuaStateView& state) {
    int result = 1;
    if (memberName=="loops") {
        LuaTable table(state, false);
        for (auto& pair : loops) {
            table.set<LuaNativeString,FeedbackLoop*>(pair.first.c_str(), pair.second.get());
        }
    } else {
        result = 0;
    }
    return result;
}

void FeedbackAI::stepSimulation() {
    for (auto& pair : loops) {
        pair.second->stepSimulation();
    }
}

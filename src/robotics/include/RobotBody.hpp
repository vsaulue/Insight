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

#ifndef ROBOTBODY_HPP
#define ROBOTBODY_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "AIInterface.hpp"
#include "Body.hpp"
#include "Joint.hpp"
#include "JointInfo.hpp"
#include "lua/types/LuaTable.hpp"
#include "lua/types/LuaVirtualClass.hpp"
#include "World.hpp"

/**
 * Object creating all robot parts and their constraints.
 *
 * Body parts are owned by the physics engine. Only references are kept inside
 * this object.
 */
class RobotBody : public LuaVirtualClass {
public:
    struct ConstructionInfo {
    public:
        /** internal information to build new joints. */
        struct JointData {
            /** Unique name of the joint. */
            std::string jointName;
            /** Construction info about this type of joint. */
            std::shared_ptr<const JointInfo> jointInfo;
            /** Name of the body part holding the convex part of the joint. */
            std::string convexPartName;
            /** Name of the body part holding the concave part of the joint. */
            std::string concavePartName;
            /** Flag set when the convex part should be moved to match the position of the concave part. */
            bool placeConvex;
        };

        /** Input data of a given joint. */
        struct JointInputData {
            /** Construction info about this type of joint. */
            std::shared_ptr<const JointInfo> info;
            /** Name of the body part holding the convex part of the joint. */
            std::string convexPartName;
            /** Name of the body part holding the concave part of the joint. */
            std::string concavePartName;
        };

        /**
         * Creates a new ConstructionInfo object.
         * @param parts Map of body part shapes, indexed by their names in the RobotBody.
         * @param basePartName Name of the base part in the RobotBody.
         * @param joints Map of tuples <JointInfo, convexPartName, concavePartName>, indexed by the joint name.
         */
        ConstructionInfo(const std::unordered_map<std::string, std::shared_ptr<Shape>>& parts,
                         const std::string& basePartName,
                         const std::unordered_map<std::string, JointInputData>& joints);

        /**
         * Gets the set of body parts names and their shapes.
         * @return The map of body part shapes, indexed bi their names.
         */
        const std::unordered_map<std::string, std::shared_ptr<Shape>> getParts() const;

        /**
         * Gets the name of the base body part.
         * @return The name of the base body part.
         */
        const std::string& getBasePartName() const;

        /**
         * Gets a vector containing construction data for the joints.
         *
         * The joints has been sorted in infixed depth-first order from the base
         * body part. Body parts can be attached one by one following this order
         * to ensure correct final position relative to each other.
         *
         * @return A vector containing construction data for the joints.
         */
        const std::vector<JointData>& getJoints() const;
    private:

        /** Map of body part shapes, indexed by their names. */
        std::unordered_map<std::string, std::shared_ptr<Shape>> parts;
        /** Name of the base part. */
        std::string basePartName;
        /** Set of Joint construction data (infixed depth-first order from the root). */
        std::vector<JointData> joints;
    };

    /**
     * Creates a new robot body.
     * @param world World in which the body will be created.
     * @param info Construction info for the robot.
     */
    RobotBody(World& world, const ConstructionInfo& info);

    virtual ~RobotBody();

    int luaIndex(const std::string& memberName, LuaStateView& state) override;

    /**
     * Constructs a new RobotBody from a Lua table.
     * @param table Table containing the parameters of the new RobotBody.
     * @return The new RobotBody object.
     */
    static std::unique_ptr<RobotBody> luaGetFromTable(LuaTable& table);
private:
    /** Set of body parts, indexed by their names. */
    std::unordered_map<std::string, std::shared_ptr<Body>> parts;
    /** Set of joints between body parts, indexed by their names. */
    std::unordered_map<std::string, std::shared_ptr<Joint>> joints;
    /** Reference body (position & rotation of the RobotBody is the one of this body). */
    Body* baseBody;
    /** Interface (input/output signals) for an AI. */
    AIInterface aiInterface;
};

#endif /* ROBOTBODY_HPP */

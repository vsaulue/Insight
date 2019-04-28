
local Demo = require("demos/Demo")

local demoRobots = Demo.new({
    name= "robots",
    description= "Stability test of various robots, with linear feedback control laws.",
})

local function newRobotData(infoName, pos)
    return {info= require("robots/"..infoName), position= pos}
end

function demoRobots.exec()
    Demo.newTerrain()
    Demo.placeCamera({0,1,8},{0,0,0})

    local RobotsData= {
        ballBot= newRobotData("ballBotInfo", {-3,0.4,0}),
        stableBallBot = newRobotData("stableBallBotInfo", {-1,0.4,0}),
        android = newRobotData("androidInfo", {1,1.25,0}),
        pendulum = newRobotData("pendulumInfo", {4,3,0}),
    }

    robots = {}
    for name,data in pairs(RobotsData) do
        print("New robot: " .. name)
        robot = insight:newRobot(data.info,{type="feedback"})
        robot.body:setPosition(data.position)
        robots[name] = robot
    end

    insight:resume()
end

return demoRobots
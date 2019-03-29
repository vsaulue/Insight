
local allDemos= {}

local SCRIPT_NAMES= {
    "demoPhysics",
    "demoRobots",
    "demoShapes",
}

for _,script_name in ipairs(SCRIPT_NAMES) do
    local demo = require("demos/".. script_name)
    allDemos[demo.name] = demo
end

return allDemos

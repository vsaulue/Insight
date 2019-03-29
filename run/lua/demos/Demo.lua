
local newTerrain = require("newTerrain")

local Demo= {
    __index= {
        -- Name of this demo
        name= "Unknown",
        -- Description of what this demo does/shows
        description= "Unknown",
        -- Function called to launch this demo.
        exec= function()

        end,
    },

    -- Flag preventing the launch of 2 demos.
    running = false,
}

-- Creates a new demo.
--
-- Arguments:
-- * initValue: a table containing the initial values for each field.
--
-- Returns: the created object (inputValue if it was a table, a new table otherwise)
function Demo.new(initValue)
    result = initValue or {}
    setmetatable(result, Demo)
    return result
end

-- Creates an immobile horizontal plane to act as the ground.
function Demo.newTerrain()
    print("Creating ground...")
    return newTerrain()
end

-- Places the camera in the scene
--
-- Arguments:
-- * position: coordinates where the camera will be placed.
-- * lookAt: coordinates of a point that will be centered in the view.
function Demo.placeCamera(position, lookAt)
    local camera = insight.graphicEngine.camera
    camera:setPosition(position)
    camera:lookAt(lookAt)
end

function Demo.__call(self, ...)
    if Demo.running then
        print("A demo is already running. Restart the program to run another one.")
    else
        Demo.running = true
        if self.name then
            print("Launching " .. self.name .. " demo")
            print("")
        end
        result = {self.exec(...)}
        print("")
        return table.unpack(result)
    end
end

function Demo.__tostring(self)
    return self.name .. ": " .. self.description
end

return Demo

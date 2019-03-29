
-- Creates an infinite horizontal plane acting as the ground
local function newTerrain()
    return insight.world:newBody({
        shape= {
            type= "StaticPlane",
            params= {
                normal= {0,1,0}, -- normal vector of the plane.
                offset= 0        -- the point origin+offset*normal is in the plane (here {0,0,0})
            },
        },
    })
end

return newTerrain
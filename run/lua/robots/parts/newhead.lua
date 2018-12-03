
local shapes = require("shapes")

local newShape = insight.world.newShape
local BULLET_MARGIN = insight.world.defaultMargin.value

-- Creates a head shapes of the specified dimensions.
--
-- Arguments:
-- * headRadius: radius of the half sphere
-- * neckRadius: radius of the neck (the part on which the head will be placed)
-- * neckLength: distance between the half sphere and the surface of the neck
-- * density: density of the created shapes
-- Returns: a shape of the desired dimensions
local function newHead(headRadius, neckRadius, neckLength, density)
    local children = {}
    table.insert(children,{
        transform= {rotation= {0,0,0,1}, position= {0, 0, 0}},
        shape= shapes.newHalfSphere(headRadius,4,16,true,density),
    })
    table.insert(children,{
        transform= {rotation= {axis={1,0,0}, angle= math.pi/2}, position= {0, 0, headRadius/2+0.01}},
        shape= {
            type= "Cylinder",
            params= {
                density= density,
                halfExtents= {headRadius/4, headRadius/2, headRadius/4},
            }
        }
    })
    local neckThickness = 0.1*headRadius
    local neckRealLength = neckLength + neckRadius*(1-math.sqrt(1-math.pow((headRadius-neckThickness/2)/neckRadius,2))) - BULLET_MARGIN
    table.insert(children, {
        transform= {rotation= {0,0,0,1}, position= {0, -3/8*headRadius-neckRealLength/2, 0}},
        shape= shapes.newTube(headRadius+BULLET_MARGIN, neckRealLength, neckThickness, 16, density),
    })
    return newShape({type="Compound", params= {children= children}})
end

return newHead

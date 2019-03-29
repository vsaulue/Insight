-- Demo of the physics engine

local Demo = require("demos/Demo")

local demoPhysics = Demo.new({
    name= "physics",
    description= "Physics engine demonstration. Creates a pyramid of cuboids, and launch a ball on it.",
})

local world= insight.world

-- Space between blocks in the pyramid
local MARGIN= 0.005

-- Creates a row of cuboids along the X axis.
--
-- Arguments:
-- * number: number of cuboids to create
-- * firstPos: coordinates of the 1st cuboid
-- * rotation: quaternion encoding the orientation of the cuboids
-- * blockShape: shape of the cuboids
-- * step: distance between 2 cuboids in the row
local function newCuboidRow(number, firstPos, rotation, blockShape, step)
    local pos= {firstPos[1], firstPos[2], firstPos[3]}
    for i=0,number do
        newBloc= world:newBody({
            shape= blockShape,
        })
        pos[1]= pos[1] + step + 2*MARGIN
        newBloc:setPosition(pos)
        newBloc:setRotation(rotation)
    end
end

-- Creates a row of cuboids along the X axis.
--
-- Arguments:
-- * floorCount: Height of the cuboid stack
-- * block_dims: dimensions of each cuboid
local function pyramid(floorCount, block_dims)
    local blockShape= world.newShape({
        type= "Cuboid",
        params={
            halfExtents= block_dims,
            density= 1000,
        },
    })
    local pos = {0, block_dims[2], 0}
    for i=floorCount,1,-1 do
        newCuboidRow(i, pos, {x=0, y=0, z=0, w=1}, blockShape, 2*block_dims[2])
        pos[1]= pos[1] + MARGIN + block_dims[2]
        pos[2]= pos[2] + block_dims[2] + block_dims[1]
        newCuboidRow(i-1,pos,{axis={0,0,1}, angle=math.pi/2}, blockShape, 2*block_dims[2])
        pos[2]= pos[2] + block_dims[2] + block_dims[1]
    end
end

function demoPhysics.exec()
    Demo.placeCamera({0,15,24}, {10,10,0})
    Demo.newTerrain()

    -- Pyramid
    print("Creating a new pyramid of cuboids...")
    pyramid(20, {0.1, 0.5, 0.15})

    -- Ball
    print("Creating a sphere with an initial velocity... (variable: ball)")
    ball = world:newBody({
        shape= {
            type= "Sphere",
            params= {
                radius=1,
                density=5000,
            }
        }
    })
    ball:setPosition({10,1.1,12})
    ball:setVelocity({0,0,-2})

    print("Starting simulation")
    insight:resume()
end

return demoPhysics
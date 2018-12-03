
-- Tables containing function to create geometry in the physics engine.
local shapes = {}

local newShape = insight.world.newShape

-- Creates the shape of a half sphere.
--
-- The half sphere is cut on the XZ plane. The sphere is made of vertices with Y+ coordinates.
--
-- * radius: radius of the sphere
-- * pCount: number of parallels (resolution)
-- * mCount: number of meridians (resolution)
-- * useCenterOfMass: true to use the center of mass as origin, false to keep the geometric center
-- * density: density of the material
-- Returns: a convex hull shape representing a half sphere.
function shapes.newHalfSphere(radius, pCount, mCount, useCenterOfMass, density)
    local vertices = {{0,radius,0}}
    local loop = {}
    for i=0,mCount-1 do
        local angle = i/mCount * 2*math.pi
        table.insert(loop, {math.cos(angle), math.sin(angle)})
    end
    for j=0,pCount-1 do
        local angle = j/pCount * math.pi/2
        local loopRadius = radius*math.cos(angle)
        local y = radius*math.sin(angle)
        for i,point in ipairs(loop) do
            table.insert(vertices, {loopRadius*point[1], y, loopRadius*point[2]})
        end
    end
    if useCenterOfMass then
        for index,value in ipairs(vertices) do
            value[2] = value[2] - 3/8 * radius
        end
    end
    return newShape({
        type= "ConvexHull",
        params= {
            mass= math.pow(radius, 3) *2/3*math.pi * density,
            vertices= vertices,
        },
    })
end

-- Creates the shape of a tube.
--
-- Y axis, origin is the center of mass.
--
-- * radius: external radius of the tube (XZ plane)
-- * length: length of the tube (Y axis)
-- * thickness: width of the wall
-- * vCount: Number of faces (resolution)
-- * density: density of the material
-- Returns: a compound shape representing a tube.
function shapes.newTube(radius, length, thickness, vCount, density)
    local halfExtents = {
        thickness/2,
        length/2,
        radius*math.abs(math.sin(math.pi/vCount)),
    }
    shape=insight.world.newShape({type="Cuboid", params={density= density, halfExtents= halfExtents}})
    local comRadius= radius*math.abs(math.cos(math.pi/vCount))-halfExtents[1]
    local parts = {}
    for i=0,vCount-1 do
        local angle= (2*i+1)/vCount*math.pi
        table.insert(parts, {
            transform= {
                rotation= {axis= {0,1,0}, angle= angle},
                position= {comRadius*math.cos(angle), 0, -comRadius*math.sin(angle)}
            },
            shape= shape,
        })
    end
    return newShape({
        type= "Compound",
        params= {children= parts},
    })
end

return shapes

-- Example of script defining a simple world.
-- Units:
-- - distances & coordinates : m
-- - mass : kg
-- - density : kg/m^3


local world = insight.world

-- Currently 4 different basic shapes can be inserted via Lua.
-- 1) StaticPlane shape (cannot move)
terrain = world:newBody({
    shape= {
        type= "StaticPlane",
        params= {
            normal= {0,1,0}, -- normal vector of the plane.
            offset= -2       -- the point origin+offset*normal is in the plane (here {0,-2,0})
        },
    },
})
terrain:setPosition({0, -2, 0})

-- 2) Sphere shape
sphere = world:newBody({
    shape= {
        type= "Sphere",
        params= {
            radius= 0.5,
            density= 1000,
            -- mass can be specified instead of density.
        },
    },
})
sphere:setPosition({0.3, 2, 0.1})

-- 3) Cylinder shape
-- In default orientations, the cylinder is Y-axis aligned.
cylinder = world:newBody({
    shape= {
        type="Cylinder",
        params= {
            halfExtents= {1, 0.5, 1}, -- 2m radius, 1m height.
            mass= 15000
            -- density can be specified instead of mass.
        },
    },
})
cylinder:setPosition({-1, 0, 2})

-- 4) Cuboid shape
cuboid = world:newBody({
    shape= {
        type= "Cuboid",
        params= {
            halfExtents= {0.2,0.7,0.3},
            density= 1200,
            -- mass can be specified instead of density.
        },
    },
})
cuboid:setPosition({-1,3,2})
-- Example of script defining a simple world.
-- Units:
-- - distances & coordinates : m
-- - mass : kg
-- - density : kg/m^3

-- List of object added into the world by this script.
objects= {}

local world = insight.world

-- Currently 4 different basic shapes can be inserted via Lua.
-- 1) StaticPlane shape (cannot move)
objects.terrain = world:newBody({
    shape= {
        type= "StaticPlane",
        params= {
            normal= {0,1,0}, -- normal vector of the plane.
            offset= -2       -- the point origin+offset*normal is in the plane (here {0,-2,0})
        },
    },
})

-- 2) Sphere shape
objects.sphere = world:newBody({
    shape= {
        type= "Sphere",
        params= {
            radius= 0.5,
            density= 1000,
            -- mass can be specified instead of density.
        },
    },
})
objects.sphere:setPosition({0.3, 2, 0.1})

-- 3) Cylinder shape
-- In default orientations, the cylinder is Y-axis aligned.
objects.cylinder = world:newBody({
    shape= {
        type="Cylinder",
        params= {
            halfExtents= {1, 0.5, 1}, -- 2m radius, 1m height.
            mass= 15000
            -- density can be specified instead of mass.
        },
    },
})
objects.cylinder:setPosition({-1, 0, 2})

-- 4) Cuboid shape
objects.cuboid = world:newBody({
    shape= {
        type= "Cuboid",
        params= {
            halfExtents= {0.2,0.7,0.3},
            density= 1200,
            -- mass can be specified instead of density.
        },
    },
})
objects.cuboid:setPosition({-1,3,2})

-- It is possible to combine the 4 basic shapes into a compound shape:
objects.table = world:newBody({
    shape= {
        type= "Compound",
        params= {
            children= {
                {
                    transform={position={0,0.25,0}, rotation={0,0,0,1}},
                    shape= {type="Cylinder", params={density=2000, halfExtents={1.5,0.075,1.5}}},
                },{
                    transform={position={0,-1,0}, rotation={0,0,0,1}},
                    shape= {type="Cuboid", params={density=2000, halfExtents={0.5,0.1,0.5}}},
                },{
                    transform={position={0,-0.375,0}, rotation={0,0,0,1}},
                    shape= {type="Cylinder", params={density=2000, halfExtents={0.1,0.65,0.1}}},
                },
            },
        },
    },
})
objects.table:setPosition({2,-0.90,5})

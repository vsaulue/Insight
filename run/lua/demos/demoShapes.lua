-- Example of script defining a simple world.
--
-- Units:
-- - distances & coordinates : m
-- - mass : kg
-- - density : kg/m^3

local Demo = require("demos/Demo")

local demoShapes = Demo.new({
    name= "shapes",
    description= "List of possible shapes in the physics engine.",
})

function demoShapes.exec()
    Demo.placeCamera({0,1,6},{0,1,0})
    local world = insight.world

    -- List of object added into the world by this script.

    print("Currently 5 different basic shapes can be inserted via Lua.")
    objects= {}

    print("1 - StaticPlane shape (cannot move), used for the ground")
    objects.terrain = insight.world:newBody({
        shape= {
            type= "StaticPlane",
            params= {
                normal= {0,1,0}, -- normal vector of the plane.
                offset= 0        -- the point origin+offset*normal is in the plane (here {0,0,0})
            },
        },
    })

    print("2 - Sphere shape")
    objects.sphere = world:newBody({
        shape= {
            type= "Sphere",
            params= {
                radius= 0.3,
                density= 1000,
                -- mass can be specified instead of density.
            },
        },
    })
    objects.sphere:setPosition({3, 0.5, 0})

    print("3 - Cylinder shape")
    -- In default orientations, the cylinder is Y-axis aligned.
    objects.cylinder = world:newBody({
        shape= {
            type="Cylinder",
            params= {
                halfExtents= {1, 0.5, 1}, -- 2m radius, 1m height.
                mass= 15000,
                -- density can be specified instead of mass.
            },
        },
    })
    objects.cylinder:setPosition({1, 0.5, 0})

    print("4 - Cuboid shape")
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
    objects.cuboid:setPosition({-1,0.35,0})

    print("5 - Convex hull shape (here a pyramid)")
    objects.pyramid = world:newBody({
        shape= {
            type= "ConvexHull",
            params= {
                mass= 200,
                vertices= {
                    {-0.5,-0.2,-0.5},
                    {-0.5,-0.2, 0.5},
                    { 0.5,-0.2, 0.5},
                    { 0.5,-0.2,-0.5},
                    { 0, 0.4, 0},
                },
            },
        },
    })
    objects.pyramid:setPosition({-2,0.2,0})

    print("")
    print("It is possible to combine the basic shapes to make concave objects (here a table)")
    objects.table = world:newBody({
        shape= {
            type= "Compound",
            params= {
                children= {
                    {
                        transform={position={0,0.2,0}, rotation={0,0,0,1}},
                        shape= {type="Cylinder", params={density=2000, halfExtents={0.75,0.05,0.75}}},
                    },{
                        transform={position={0,-0.6,0}, rotation={0,0,0,1}},
                        shape= {type="Cuboid", params={density=2000, halfExtents={0.3,0.075,0.3}}},
                    },{
                        transform={position={0,-0.2,0}, rotation={0,0,0,1}},
                        shape= {type="Cylinder", params={density=2000, halfExtents={0.075,0.4,0.075}}},
                    },
                },
            },
        },
    })
    objects.table:setPosition({-3, 0.675, 0})

    print("")
    print("All objects were pushed in the 'objects' global variable.")
end

return demoShapes
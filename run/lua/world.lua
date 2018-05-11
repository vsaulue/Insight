-- Example of script defining a simple world.

local world = insight.world

terrain = world:newStaticPlane({0,1,0},0)
terrain:setPosition({0, -2, 0})

sphere1 = world:newSphere(1,1)
sphere2 = world:newSphere(0.125,0.5)
sphere2:setPosition({0.3, 2, 0.1})

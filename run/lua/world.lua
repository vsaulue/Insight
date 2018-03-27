-- Example of script defining a simple world.

local world = insight.world

terrain = world:newTerrain()
terrain:setPosition(0,-2,0)

sphere1 = world:newSphere()
sphere2 = world:newSphere()
sphere2:setPosition(0.3,2,0.1)

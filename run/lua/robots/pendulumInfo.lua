-- Example of script defining construction info for a simple pendulum,
-- without damping, and with an engine on the axis.

local pendulumInfo = insight.newRobotInfo({
    -- list of body parts (physics engine shapes), indexed by their names.
    parts= {
        Base= {
            type= "Cuboid",
            params= {
                halfExtents= {0.5,0.5,0.5},
                mass= 0, -- 0 = infinite mass: can't move
            },
        },
        Pendulum= {
            type= "Compound",
            params= {
                children= {
                    {
                        transform= {rotation={axis={1,0,0},angle=math.pi/2}, position={0,-0.30,0}},
                        shape= {type="Cylinder", params={density=2000, halfExtents={0.25,0.1,0.25}}},
                    },{
                        transform= {rotation={0,0,0,1}, position={0,0.45,0}},
                        shape= {type="Cylinder", params={density=2000, halfExtents={0.075,0.75,0.075}}},
                    },
                },
            },
        },
    },
    -- Name of the reference part (position of the whole robot is manipulated with this object).
    basePart= "Base",
    -- List of joints (how the different parts are attached to each other).
    joints= {
        Axis= {
            -- The two parts linked by this joint.
            convexPart= "Pendulum",
            concavePart= "Base",
            info= {
                -- 1 degree of freedom joint (x: free axis).
                type= "Cylindric",
                params= {
                    -- Where the joint is located on each part.
                    -- In the frames of the transform, the x-axis is the joint axis.
                    convexTransform={
                        rotation={axis={0,1,0},angle=math.pi/2},
                        position={0, 1.2, 0}
                    },
                    concaveTransform={
                        rotation={axis={0,1,0},angle=math.pi/2},
                        position={0, 0, 0.65}
                    },

                    -- Adds a cylinder on the axis of the "Pendulum" object.
                    generateConvexShape= true,
                    density= 1500,
                    radius= 0.075,
                    length= 0.2,

                    -- Other constraints of this joint.
                    startRotation= 1,
                    minAngle= -math.huge,
                    maxAngle= math.huge,
                    maxMotorTorque= 20,
                    frictionCoefficient= 0,
                },
            },
        },
    },
})

return pendulumInfo

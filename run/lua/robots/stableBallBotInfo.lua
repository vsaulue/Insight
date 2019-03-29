-- Example of script defining construction info for a simple 2 part robot.

-- Helper function to create the geometry of the head.
local newHead = require("robots/parts/newHead")

-- Various dimensions of the robot parts.
local BODY_RADIUS = 0.35
local HEAD_RADIUS = 0.2
local NECK_LENGTH = 0.05
local MARGIN = 0.01

local stableBallBotInfo= insight.newRobotInfo{
    -- list of body parts (physic engine shapes), indexed by their names.
    parts= {
        Torso= {
            type= "Sphere",
            params= {
                radius= BODY_RADIUS,
                density= 1000,
            }
        },
        Head= {
            type= "Compound",
            params={
                children= {
                    {
                        shape= newHead(HEAD_RADIUS, BODY_RADIUS, NECK_LENGTH-MARGIN, 1000),
                        transform= {rotation={0,0,0,1}, position={0,1.2*BODY_RADIUS+NECK_LENGTH+3/8*HEAD_RADIUS,0}},
                    },
                },
            },
        },
    },
    -- name of the base part (this is the position read & written by Lua methods)
    basePart= "Torso",
    -- list of joints indexed by their names.
    joints= {
        Neck= {
            -- Names of the 2 body parts of this joint.
            convexPart= "Torso",
            concavePart= "Head",
            info= {
                -- 3 degree of freedom joint (x: twist axis).
                type= "Spherical",
                params= {
                    -- Position of the pivot points relative to each part.
                    convexTransform= {
                        rotation= {axis= {0,0,1}, angle= math.pi/2},
                        position= {0, 0, 0},
                    },
                    concaveTransform= {
                        rotation= {axis={0,0,1}, angle= math.pi/2},
                        position= {0, 0.2*BODY_RADIUS, 0},
                    },

                    -- Initial relative orientation of the 2 parts.
                    startRotation= {0,0,0,1},

                    -- Maximum angle about each axis from the neutral position (> PI: no restriction).
                    limits= {math.huge, math.huge, math.huge},

                    -- Maximum motor torque about each axis.
                    maxMotorTorque= {1, 5, 5},

                    -- Damping coefficients (higher -> more friction).
                    frictionCoefficients= {0.25, 0.25, 0.25},

                    -- Disable automatic addition of joint geometry to the parts.
                    generateConvexShape= false,
                    -- Density of created parts (not used here).
                    density= 1000,
                    -- Radius of the ball of the joint (not used here).
                    radius= 0,
                },
            },
        },
    },
}

return stableBallBotInfo

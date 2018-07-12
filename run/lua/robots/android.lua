-- Example of script defining construction info for an android (IN PROGRESS).

-- Density of the body parts (kg/m^3).
local SHAPE_DENSITY = 1500
-- Density of the generated joint parts (kg/m^3).
local JOINT_DENSITY = 1200
-- Distance between the surface of the convex & concave parts of a joint (m).
local MARGIN = 0.005

-- Radius of the half sphere of the head (m).
local HEAD_RADIUS = 0.1
-- Length of the neck (concave part attached to the head).
local NECK_LENGTH = 0
-- Dimensions of the cylindric part of the arm.
local ARM_HALF_EXTENTS = {0.028, 0.10, 0.028}
-- Dimensions of the longest cylindric part of the forearm.
local FOREARM_HALF_EXTENTS = {0.028, 0.11, 0.028}
-- Dimensions of the cylindric part of the thigh.
local THIGH_HALF_EXTENTS = {0.035, 0.175, 0.035}
-- Dimensions of the cylindric part of the leg.
local LEG_HALF_EXTENTS = {0.035, 0.16, 0.035}
-- Dimensions of the cuboid of the foot.
local FOOT_HALF_EXTENTS = {0.06, 0.03, 0.09}
-- Dimension of the cuboid of the toes.
local TOES_HALF_EXTENTS = {0.06, 0.03, 0.04}
-- Dimensions of the cuboid of the hand.
local HAND_HALF_EXTENTS = {0.02, 0.06, 0.04}
-- Dimensions of the cylinder of the torso.
local TORSO_HALF_EXTENTS = {0.15, 0.15, 0.15}

-- Shoulder ball radius (m).
local SHOULDER_BALL_RADIUS = 0.065
-- Radius of the cylindric part of the elbow (m).
local ELBOW_CYLINDER_RADIUS = 0.03
-- Hip joint ball radius (m).
local HIP_BALL_RADIUS = 0.05
-- Neck joint ball radius (m).
local NECK_BALL_RADIUS = 0.15
-- Radius of the cylindric part of the knee (m).
local KNEE_CYLINDER_RADIUS = 0.05
-- Ankle ball radius (m).
local ANKLE_BALL_RADIUS = 0.05
-- Wrist joint ball radius (m).
local WRIST_BALL_RADIUS = 0.03

local BULLET_MARGIN = insight.world.defaultMargin.value

local newShape = insight.world.newShape

-- Generates a half sphere (cut on the XZ plane, Y+ part kept).
local function halfSphere(radius)
    local vertices = {{0,radius,0}}
    local loop = {}
    for i=0,15 do
        local angle = i/8 * math.pi
        table.insert(loop, {math.cos(angle), math.sin(angle)})
    end
    for j=0,3 do
        local angle = j/8 * math.pi
        local loopRadius = radius*math.cos(angle)
        local y = radius*math.sin(angle)
        for i,point in ipairs(loop) do
            table.insert(vertices, {loopRadius*point[1], y, loopRadius*point[2]})
        end
    end
    return vertices
end

-- Generates the "neck" part of the head (concave part of the joint).
local function neckSocketParts(parts, headRadius, neckRadius, neckLength)
    local partWidth = 0.1*headRadius
    local halfExtents = {
        partWidth/2,
        0.5*(neckLength + neckRadius*(1-math.sqrt(1-math.pow((headRadius-partWidth/2)/neckRadius,2))) - MARGIN - BULLET_MARGIN),
        0.5*(headRadius+BULLET_MARGIN)*math.sqrt(2*(1-math.cos(math.pi/8))),
    }
    shape=newShape({type="Cuboid", params={density= SHAPE_DENSITY, halfExtents= halfExtents}})
    for i=0,15 do
        local angle= (2*i+1)/16*math.pi
        local radius= headRadius*math.sqrt((math.cos(math.pi/8)+1)/2)-halfExtents[1]+BULLET_MARGIN
        table.insert(parts, {
            transform= {
                rotation= {axis= {0,1,0}, angle= angle},
                position= {radius*math.cos(angle), -3/8*headRadius-halfExtents[2], -radius*math.sin(angle)}
            },
            shape= shape,
        })
    end
end

-- Generates the shape of the head.
function head_shape()
    local shapes = {}
    table.insert(shapes,{
        transform= {rotation= {0,0,0,1}, position= {0, -3/8*HEAD_RADIUS, 0}},
        shape= {
            type= "ConvexHull",
            params= {
                mass= math.pow(HEAD_RADIUS, 3) *2/3*math.pi * SHAPE_DENSITY,
                vertices= halfSphere(HEAD_RADIUS),
            }
        }
    })
    table.insert(shapes,{
        transform= {rotation= {axis={1,0,0}, angle= math.pi/2}, position= {0, 0, HEAD_RADIUS/2+0.01}},
        shape= {
            type= "Cylinder",
            params= {
                density= SHAPE_DENSITY,
                halfExtents= {0.025, HEAD_RADIUS/2, 0.025},
            }
        }
    })
    neckSocketParts(shapes, HEAD_RADIUS, NECK_BALL_RADIUS, NECK_LENGTH)
    return newShape({type="Compound", params= {children= shapes}})
end

local SHAPES= {
    Head= newShape(head_shape()),
    Torso= newShape{type= "Cylinder", params= {density= SHAPE_DENSITY, halfExtents= TORSO_HALF_EXTENTS}},
    Arm= newShape{type= "Cylinder", params= {density= SHAPE_DENSITY, halfExtents= ARM_HALF_EXTENTS}},
    Forearm= newShape{type= "Cylinder", params= {density= SHAPE_DENSITY, halfExtents= FOREARM_HALF_EXTENTS}},
    Thigh= newShape{type= "Cylinder", params= {density= SHAPE_DENSITY, halfExtents= THIGH_HALF_EXTENTS}},
    Leg= newShape{type= "Cylinder", params= {density= SHAPE_DENSITY, halfExtents= LEG_HALF_EXTENTS}},
    Foot= newShape{type= "Cuboid", params= {density= SHAPE_DENSITY, halfExtents= FOOT_HALF_EXTENTS}},
    Toes= newShape{type= "Cuboid", params= {density= SHAPE_DENSITY, halfExtents= TOES_HALF_EXTENTS}},
    Hand= newShape{type= "Cuboid", params= {density= SHAPE_DENSITY, halfExtents= HAND_HALF_EXTENTS}},
}

local JOINTS_INFO = {
    Neck= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,0,1}, angle= math.pi/2},
                position= {0, TORSO_HALF_EXTENTS[2], 0},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {axis={0,0,1}, angle= math.pi/2},
                position= {0, -NECK_BALL_RADIUS-(3/8)*HEAD_RADIUS-NECK_LENGTH-MARGIN, 0},
            },
            radius= NECK_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits= {math.pi/2, math.pi/3, math.pi/3},
        },
    },
    LeftShoulder= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,1,0}, angle= -3*math.pi/4},
                position= {-TORSO_HALF_EXTENTS[1]-0.75*SHOULDER_BALL_RADIUS, 0.9*TORSO_HALF_EXTENTS[2], 0},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {0.5,-0.5,-0.5,0.5},
                position= {0, ARM_HALF_EXTENTS[2]+SHOULDER_BALL_RADIUS+MARGIN, 0},
            },
            radius= SHOULDER_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits= {math.pi/3, math.pi/2, math.pi/2},
        },
    },
    RightShoulder= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,1,0}, angle= -math.pi/4},
                position= {TORSO_HALF_EXTENTS[1]+0.75*SHOULDER_BALL_RADIUS, 0.9*TORSO_HALF_EXTENTS[2], 0},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {0.5,-0.5,-0.5,0.5},
                position= {0, ARM_HALF_EXTENTS[2]+SHOULDER_BALL_RADIUS+MARGIN, 0},
            },
            radius= SHOULDER_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits= {math.pi/3, math.pi/2, math.pi/2},
        },
    },
    LeftElbow= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={0,1,0}, angle=math.pi/2}, position={0, -ARM_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={0,1,0}, angle=math.pi/2}, position={0, FOREARM_HALF_EXTENTS[2]+ELBOW_CYLINDER_RADIUS+MARGIN, 0}},
            radius= ELBOW_CYLINDER_RADIUS,
            length= ARM_HALF_EXTENTS[1]*2,
            startRotation= 0,
            minAngle= -0.1,
            maxAngle= math.pi,
        },
    },
    RightElbow= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={0,1,0}, angle=-math.pi/2}, position={0, -ARM_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={0,1,0}, angle=-math.pi/2}, position={0, FOREARM_HALF_EXTENTS[2]+ELBOW_CYLINDER_RADIUS+MARGIN, 0}},
            radius= ELBOW_CYLINDER_RADIUS,
            length= ARM_HALF_EXTENTS[1]*2,
            startRotation= 0,
            minAngle= -0.1,
            maxAngle= math.pi,
        },
    },
    Wrist= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,0,1}, angle= -math.pi/2},
                position= {0, HAND_HALF_EXTENTS[2], 0},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {axis= {0,0,1}, angle= -math.pi/2},
                position= {0, -FOREARM_HALF_EXTENTS[2]-WRIST_BALL_RADIUS-MARGIN, 0},
            },
            radius= WRIST_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits= {math.pi/4, 0, math.pi/2},
        },
    },
    LeftHip= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,0,1}, angle= math.pi/2},
                position= {-0.08, -TORSO_HALF_EXTENTS[2]-0.5*HIP_BALL_RADIUS, 0},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {axis= {0,0,1}, angle= math.pi/2},
                position= {0, THIGH_HALF_EXTENTS[2]+HIP_BALL_RADIUS+MARGIN, 0},
            },
            radius= HIP_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits= {math.pi/6, math.pi/2, math.pi/4},
        },
    },
    RightHip= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,0,1}, angle= math.pi/2},
                position= {0.08, -TORSO_HALF_EXTENTS[2]-0.5*HIP_BALL_RADIUS, 0},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {axis= {0,0,1}, angle= math.pi/2},
                position= {0, THIGH_HALF_EXTENTS[2]+HIP_BALL_RADIUS+MARGIN, 0},
            },
            radius= HIP_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits= {math.pi/6, math.pi/2, math.pi/4},
        },
    },
    Knee= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={0,0,0,1}, position={0, -THIGH_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={0,0,0,1}, position={0, LEG_HALF_EXTENTS[2]+KNEE_CYLINDER_RADIUS+MARGIN, 0}},
            radius= KNEE_CYLINDER_RADIUS,
            length= THIGH_HALF_EXTENTS[1]*2,
            startRotation= 0,
            minAngle= -math.pi,
            maxAngle= 0.1,
        },
    },
    Ankle= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform= {
                rotation= {axis= {0,0,1}, angle= -math.pi/2},
                position= {0, FOOT_HALF_EXTENTS[2]+ANKLE_BALL_RADIUS/2, -FOOT_HALF_EXTENTS[3]+ANKLE_BALL_RADIUS},
            },
            generateConvexShape= true,
            concaveTransform= {
                rotation= {axis= {0,0,1}, angle= -math.pi/2},
                position= {0, -ANKLE_BALL_RADIUS-LEG_HALF_EXTENTS[2]-MARGIN, 0},
            },
            radius= ANKLE_BALL_RADIUS,
            startRotation= {0,0,0,1},
            limits={math.pi/6, math.pi/3, math.pi/4},
        },
    },
    Toes= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={1,0,0}, angle=-math.pi/2}, position={0, 0, FOOT_HALF_EXTENTS[3]}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={1,0,0}, angle=-math.pi/2}, position={0, 0, -TOES_HALF_EXTENTS[3]-FOOT_HALF_EXTENTS[2]-MARGIN}},
            radius= FOOT_HALF_EXTENTS[2],
            length= FOOT_HALF_EXTENTS[1]*2,
            startRotation= 0,
            minAngle= -math.pi/2,
            maxAngle=  math.pi/2,
        },
    },
}

androidInfo= insight.newRobotInfo{
    -- list of body parts (physic engine shapes), indexed by their names.
    parts= {
        Torso= SHAPES.Torso,
        Head= SHAPES.Head,
        LeftArm= SHAPES.Arm,
        RightArm= SHAPES.Arm,
        LeftForearm= SHAPES.Forearm,
        RightForearm= SHAPES.Forearm,
        LeftHand= SHAPES.Hand,
        RightHand= SHAPES.Hand,
        LeftThigh= SHAPES.Thigh,
        RightThigh= SHAPES.Thigh,
        LeftLeg= SHAPES.Leg,
        RightLeg= SHAPES.Leg,
        LeftFoot= SHAPES.Foot,
        RightFoot= SHAPES.Foot,
        LeftToes= SHAPES.Toes,
        RightToes= SHAPES.Toes,
    },
    -- name of the base part (this is the position read & written by Lua methods)
    basePart= "Torso",
    -- list of joints indexed by their names.
    joints= {
        Neck= {info= JOINTS_INFO.Neck, convexPart= "Torso", concavePart= "Head"},
        LeftShoulder= {info= JOINTS_INFO.LeftShoulder, convexPart= "Torso", concavePart= "LeftArm"},
        RightShoulder= {info= JOINTS_INFO.RightShoulder, convexPart= "Torso", concavePart= "RightArm"},
        LeftElbow= {info= JOINTS_INFO.LeftElbow, convexPart= "LeftArm", concavePart= "LeftForearm"},
        RightElbow= {info= JOINTS_INFO.RightElbow, convexPart= "RightArm", concavePart= "RightForearm"},
        LeftWrist= {info= JOINTS_INFO.Wrist, convexPart= "LeftHand", concavePart= "LeftForearm"},
        RightWrist= {info= JOINTS_INFO.Wrist, convexPart= "RightHand", concavePart= "RightForearm"},
        LeftHip= {info= JOINTS_INFO.LeftHip, convexPart= "Torso", concavePart= "LeftThigh"},
        RightHip= {info= JOINTS_INFO.RightHip, convexPart= "Torso", concavePart= "RightThigh"},
        LeftKnee= {info= JOINTS_INFO.Knee, convexPart= "LeftThigh", concavePart= "LeftLeg"},
        RightKnee= {info= JOINTS_INFO.Knee, convexPart= "RightThigh", concavePart= "RightLeg"},
        LeftFoot= {info= JOINTS_INFO.Ankle, convexPart= "LeftFoot", concavePart= "LeftLeg"},
        RightFoot= {info= JOINTS_INFO.Ankle, convexPart= "RightFoot", concavePart= "RightLeg"},
        LeftToes= {info= JOINTS_INFO.Toes, convexPart= "LeftFoot", concavePart= "LeftToes"},
        RightToes= {info= JOINTS_INFO.Toes, convexPart= "RightFoot", concavePart= "RightToes"},
    },
}

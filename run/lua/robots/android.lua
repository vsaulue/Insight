-- Example of script defining construction info for an android (IN PROGRESS).

local PI = 3.14159

-- Density of the body parts (kg/m^3).
local SHAPE_DENSITY = 1500
-- Density of the generated joint parts (kg/m^3).
local JOINT_DENSITY = 1200

-- Head radius (m).
local HEAD_RADIUS = 0.5
-- Dimensions of the cylindric part of the arm.
local ARM_HALF_EXTENTS = {0.1, 0.5, 0.1}
-- Dimensions of the longest cylindric part of the forearm.
local FOREARM_HALF_EXTENTS = {0.1, 0.55, 0.1}
-- Dimensions of the cylindric part of the thigh.
local THIGH_HALF_EXTENTS = {0.15, 0.6, 0.15}
-- Dimensions of the cylindric part of the leg.
local LEG_HALF_EXTENTS = {0.15, 0.85, 0.15}
-- Dimensions of the cuboid of the foot.
local FOOT_HALF_EXTENTS = {0.225, 0.1, 0.55}
-- Dimension of the cuboid of the toes.
local TOES_HALF_EXTENTS = {0.225, 0.1, 0.15}
-- Dimensions of the cuboid of the hand.
local HAND_HALF_EXTENTS = {0.05, 0.18, 0.15}
-- Dimensions of the cylinder of the torso.
local TORSO_HALF_EXTENTS = {1,1,1}

-- Shoulder ball radius (m).
local SHOULDER_BALL_RADIUS = 0.3
-- Radius of the cylindric part of the elbow (m).
local ELBOW_CYLINDER_RADIUS = 0.12
-- Hip joint ball radius (m).
local HIP_BALL_RADIUS = 0.4
-- Neck joint ball radius (m).
local NECK_BALL_RADIUS = 1.0
-- Radius of the cylindric part of the knee (m).
local KNEE_CYLINDER_RADIUS = 0.20
-- Ankle ball radius (m).
local ANKLE_BALL_RADIUS = 0.2
-- Wrist joint ball radius (m).
local WRIST_BALL_RADIUS = 0.125

local newShape = insight.world.newShape
local SHAPES= {
    Head= newShape{type= "Sphere", params= {density= SHAPE_DENSITY, radius= HEAD_RADIUS}},
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
            convexTransform={rotation={1,0,0,0}, position={0, 1, 0}},
            generateConvexShape= true,
            concaveTransform={rotation={1,0,0,0}, position={0, -NECK_BALL_RADIUS-HEAD_RADIUS, 0}},
            radius= NECK_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    LeftShoulder= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={0,0,0,1}, position={-1.08, 0.7, 0}},
            generateConvexShape= true,
            concaveTransform={rotation={0,0,0,1}, position={0, ARM_HALF_EXTENTS[2]+SHOULDER_BALL_RADIUS, 0}},
            radius= SHOULDER_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    RightShoulder= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={0,0,0,1}, position={1.08, 0.7, 0}},
            generateConvexShape= true,
            concaveTransform={rotation={0,0,0,1}, position={0, ARM_HALF_EXTENTS[2]+SHOULDER_BALL_RADIUS, 0}},
            radius= SHOULDER_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    LeftElbow= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={0,1,0}, angle=PI/2}, position={0, -ARM_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={0,1,0}, angle=PI/2}, position={0, ARM_HALF_EXTENTS[2]+ELBOW_CYLINDER_RADIUS, 0}},
            radius= ELBOW_CYLINDER_RADIUS,
            length= ARM_HALF_EXTENTS[1]*2,
            startRotation= 0,
        },
    },
    RightElbow= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={0,1,0}, angle=-PI/2}, position={0, -ARM_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={0,1,0}, angle=-PI/2}, position={0, ARM_HALF_EXTENTS[2]+ELBOW_CYLINDER_RADIUS, 0}},
            radius= ELBOW_CYLINDER_RADIUS,
            length= ARM_HALF_EXTENTS[1]*2,
            startRotation= 0,
        },
    },
    Wrist= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={0,0,1}, angle=PI}, position={0, HAND_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={0,0,1}, angle=PI}, position={0, -ARM_HALF_EXTENTS[2]-WRIST_BALL_RADIUS, 0}},
            radius= WRIST_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    LeftHip= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={0,0,0,1}, position={-0.6, -1, 0}},
            generateConvexShape= true,
            concaveTransform={rotation={0,0,0,1}, position={0, THIGH_HALF_EXTENTS[2]+HIP_BALL_RADIUS, 0}},
            radius= HIP_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    RightHip= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={0,0,0,1}, position={0.6, -1, 0}},
            generateConvexShape= true,
            concaveTransform={rotation={0,0,0,1}, position={0, THIGH_HALF_EXTENTS[2]+HIP_BALL_RADIUS, 0}},
            radius= HIP_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    Knee= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={0,0,0,1}, position={0, -THIGH_HALF_EXTENTS[2], 0}},
            generateConvexShape= true,
            concaveTransform={rotation={0,0,0,1}, position={0, LEG_HALF_EXTENTS[2]+KNEE_CYLINDER_RADIUS, 0}},
            radius= KNEE_CYLINDER_RADIUS,
            length= THIGH_HALF_EXTENTS[1]*2,
            startRotation= 0,
        },
    },
    Ankle= {
        type= "Spherical",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={0,0,1}, angle=PI}, position={0, 0.125, -0.2}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={0,0,1}, angle=PI}, position={0, -ANKLE_BALL_RADIUS-LEG_HALF_EXTENTS[2], 0}},
            radius= ANKLE_BALL_RADIUS,
            startRotation= {0,0,0,1},
        },
    },
    Toes= {
        type= "Cylindric",
        params= {
            density= JOINT_DENSITY,
            convexTransform={rotation={axis={1,0,0}, angle=-PI/2}, position={0, 0, FOOT_HALF_EXTENTS[3]}},
            generateConvexShape= true,
            concaveTransform={rotation={axis={1,0,0}, angle=-PI/2}, position={0, 0, -TOES_HALF_EXTENTS[3]-FOOT_HALF_EXTENTS[2]}},
            radius= FOOT_HALF_EXTENTS[2],
            length= FOOT_HALF_EXTENTS[1]*2-0.01,
            startRotation= 0,
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
        LeftLeg= {info= JOINTS_INFO.Knee, convexPart= "LeftThigh", concavePart= "LeftLeg"},
        RightLeg= {info= JOINTS_INFO.Knee, convexPart= "RightThigh", concavePart= "RightLeg"},
        LeftFoot= {info= JOINTS_INFO.Ankle, convexPart= "LeftFoot", concavePart= "LeftLeg"},
        RightFoot= {info= JOINTS_INFO.Ankle, convexPart= "RightFoot", concavePart= "RightLeg"},
        LeftToes= {info= JOINTS_INFO.Toes, convexPart= "LeftFoot", concavePart= "LeftToes"},
        RightToes= {info= JOINTS_INFO.Toes, convexPart= "RightFoot", concavePart= "RightToes"},
    },
}

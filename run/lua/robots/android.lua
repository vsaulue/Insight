-- Example of script defining construction info for an android (IN PROGRESS).

android= insight.newRobotInfo{
    -- list of body parts (physic engine shapes), indexed by their names.
    parts= {
        torso= {type="Cylinder", params= {halfExtents= {1,1,1}, density=1500}},
    },
    -- name of the base part (this is the position read & written by Lua methods)
    basePart= "torso",
}

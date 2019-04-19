# Insight - Robotics library

This library provides utilities to easily constructs robots that can be interfaced with an "artificial intelligence".

# Overview

## RobotBody class

[RobotBody](include/RobotBody.hpp) is the main class of this library. It regroups:

- a set of body parts ([Body](../physics/include/Body.hpp) instances)
- a set of joints, linking body parts together
- an [AIInterface](../AI-interface/include/AIInterface.hpp) publishing the senses (joint angles) and action signals (joint torques) of this body

This class enables to insert and move the body parts and constraints into a [World](../physics/include/World.hpp), in a coherent position in relation to each other.

Lua API:

- read-only properties:
  - position: coordinates of the reference part
  - rotation: orientation of the reference part
  - aiInterface: Interface of this robot
- methods:
  - getPart: get a part by its name
  - listParts: list all the body part names of this robot
  - setPosition: sets the position of the reference part, and moves all other parts accordingly
  - setRotation: sets the orientation of the reference part, and turns all other parts accordingly
- table constructor: see [androidInfo.lua](../../run/lua/robots/androidInfo.lua) for examples

## Joint class (& derived)

This class wraps a [Constraint](../physics/include/Constraint.hpp) in order to implement a 1-DOF (degree of freedom) or 3-DOF joint in a robot. It can implement:

- limits (ex: only half a turn of amplitude)
- an internal friction
- a motor (torque generator), controlled by an [ActionSignal](../AI-Interface/ActionSignal.hpp)
- a [SenseSignal](..//AI-Interface/SenseSignal.hpp), returning the position of the joint

## JointInfo class (& derived)

This class holds the constructor data for a joint type (ex: a knee). This enables to easily instantiate multiple joints having the same properties (ex: the 2 knees of an android, the same joints between 2 robots).

Lua API:

- table constructor: see [androidInfo.lua](../../run/lua/robots/androidInfo.lua) for examples
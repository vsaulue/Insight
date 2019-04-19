# Insight - Physics engine

This engine is a wrapper of [Bullet physics](https://github.com/bulletphysics/bullet3), adding:

- RAII using smart pointers
- Lua bindings
- Event driven programming (for new entities, entities updates)
- Utilities to display the shapes of objects in a 3d graphics engine
- Units (distance, time, mass) abstraction.

# Overview

## World class

[World](include/World.hpp) is the main class of the physics engine. The world can be populated by adding:

- rigid bodies having a given shape
- constraints (ex: linking two bodies with a link)

Then it is possible to step the simulation by calling `World::stepSimulation(double)`.

Lua API:

- read-only properties:
  - gravity: acceleration vector of the gravity
  - defaultMargin: margin added to every [ConvexHullShape](include/ConvexHullShape.hpp)
- methods:
  - setGravity: changes the value of the gravity acceleration vector.
  - newBody: creates a new [Body](include/Body.hpp) and adds it to this world.
- static function:
  - newShape: creates a new [Shape](include/Shape.hpp).

## Shape class (& derived)

The [Shape](include/Shape.hpp) class is a wrapper of `btCollisionShape`, which also describe the mass and inertia moments of any body using this shape. Shape geometry should always be described with the center of mass as origin, and in the base of its principal inertia axes.

Lua API (for the abstract base class: derived classes might implement more):

- read-only properties:
  - mass: mass of this shape
  - inertia: inertia moments about the principal inertia axes
- table constructor: see the [demoShapes.lua](../../run/lua/demos/demoShapes.lua) sample script for valid input tables

## Body class

This class implement an real object in the physics engine: something having a shape, a mass, a position...

Lua API:

- read-only properties:
  - position: the coordinates of this object in the world
  - velocity: the linear velocity vector of this object
  - rotation: orientation of this object in the world, as a quaternion
  - shape: shape (with mass & inertia) of this object
- methods:
  - setPosition: sets the coordinates of this object in the world
  - setVelocity: sets the linear velocity of this object
  - setRotation: sets the orientation of this object (quaternion)

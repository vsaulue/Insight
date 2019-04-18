# Insight - AI library

This library is the place where control programs of robots are implemented. These can be AIs (machine learning approaches) or any other approach (ex: a control law used in robotics).

# Overview

## AI class

The [AI](include/AI.hpp) class is the base of control programs. Derived class must implement the `void AI::stepSimulation()` method to read the inputs of the [AIInterface](../AI-interface/include/AIInterface.hpp), and set the values of the output signals. This method will be called at regular time intervals by the simulation.

## AIFactory class

This utility class enables the construction of new [AI](include/AI.hpp) objects.

Lua API:

- table constructor: it is able to return a factory function able to build any AI class implementation, depending on the table content.

# AI implementations

## FeedbackAI

This control program is an implementation of a simplistic control law:

- each joint has an adjustable target angle/orientation
- simulation step: each joint applies independently a 2nd order linear feedback law: `torque = -A * velocity -B * (target - position)`

This control program is more to be seen as a "Hello world" example. It is currently too simplistic for many practical purposes.

Lua API:

- loops: a table of all the feedback loops (one per joint), indexed by their name. Each loop has a `target` property and an associated `setTarget` method

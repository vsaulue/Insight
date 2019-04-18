# Insight - AI interface library

This library implements the interface used by some control program (implementation is totally free: machine learning, robotic control laws...) to interact with its robot inside the simulation.


# Overview

## AIInterface

[AIInterface](include/AIInterface.hpp) class of an interface. It regroups 2 sets of signals:

- Action signals: output signals of the AI. Typically controls the direction & intensity of motors
- Sense signals: input signals of the AI. Currently used to report proprioception senses (angle / relative orientation of the two body parts of a joint).

Lua API:

- read-only properties:
  - senses: a map of all sense signals of this interface, indexed by their names.
  - actions: a map of all action signals of this interface, indexed by their names.

## Sense signal

A [SenseSignal](include/SenseSignal.hpp) object enables to read senses (input data) for the AI. The exact type depends on the type of the input data (ex: scalar, vector). It is possible to use the visitor pattern with `SenseSignal::apply(SenseVisitor&)` to get the derived type.

Lua API:

- read-only properties:
  - value: reads the input signal

## Action signal

An [ActionSignal](include/ActionSignal.hpp) object enables to set an output (ex: motor power) for the AI. The exact type depends on the type of the output data (ex: scalar, vector). It is possible to use the visitor pattern with `ActionSignal::apply(ActionVisitor&)` to get the derived type.

Lua API:

- methods
  - set: sets the value of this signal

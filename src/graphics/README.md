# Insight - Graphics engine

This engine is a wrapper of the [Irrlicht engine](http://irrlicht.sourceforge.net/), used in this program to render the simulation. Features:

- RAII via smart pointers
- Lua bindings
- Reconfigurable Keyboard/Mouse bindings
- Interface with the [physics engine](../physics): events handling (object creation & position update), accurate collision shapes

# Overview

## GraphicEngine class

[GraphicEngine](include/GraphicEngine.hpp) is the main class of this component. It will create a new window, handle keyboard & mouse inputs, and render the scene.

Lua API:

- read-only properties:
  - camera: [Camera](include/Camera.hpp) object rendering the scene
  - inputs: [InputHandler](include/InputHandler.hpp) object of the engine

## Camera class

[Camera](include/Camera.hpp) is the class wrapping the camera node of the scene. It can be controlled with the keyboard/mouse inputs, or from Lua scripts.

Lua API:

- read-only properties:
  - position: position of the camera
- methods:
  - lookAt: turns the camera in order to place the given point in the center of the view
  - setPosition: moves the camera

## User inputs

The [InputHandler](include/InputHandler.hpp) class handles Keyboard/Mouse inputs in the GUI window. More precisely:

- it stores the state of the [keyboard](include/Keyboard.hpp) & [mouse](include/Mouse.hpp) every frame
- it holds an binding table (which associate an [input event](include/InputEvent.hpp) to every [action](include/Action.hpp))
- it enables other parts of the code to check if an [action](include/Action.hpp) happened, by reading the bindings and the input devices

Current actions:

- moving the camera (default: WASD, ctrl, space)
- turning the camera (default: mouse movements), when the mouse is locked
- lock/unlock the mouse cursor in the middle of the screen (default: Q)

Lua API:

- read-only properties:
  - bindings: object holding the action <-> input map
    - listActions  method: returns the names of all existing actions
    - listEvents method: returns the name of all existing events
    - set method: assigns an event (input) to an action
    - get method: gets the event (input) triggering an action
  - settings: object holding input settings (ex: camera/rotation speed)
    - list method: lists all settings name
    - "setting name": a `Setting` object, with value/unit members, and a setValue method

See the [sample binding scripts](../../run/lua/bindings) for examples of binding definitions from Lua.

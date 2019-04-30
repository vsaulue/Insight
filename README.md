# Insight - AI experiment platform

This project aims at providing tools to test 3d robot control programs. These programs could be implemented by manually programming robotic control laws, reinforcement learning...

- Platform: Windows 64bits
- License: GPLv3
- Languages: C++17 (engine, control programs), Lua

@TODO: video/gif demo

# Install

* Download precompiled binaries from the [release page](https://github.com/vsaulue/Insight/releases)
* Unzip anywhere on your hard drive
* Launch the main program: Insight.exe

# Usage

The program currently launch 2 windows:

* A terminal, used to execute empty commands
* A 3d window, displaying the world (by default empty)

## Demo scripts

The default init script will print an help message, listing a few demo scripts. They can be executed by typing any of the following line into the console:

```
demos.shapes()
demos.physics()
demos.robots()
```

You can rewrite the default [init.lua](run/init.lua) later.

## Executing other scripts

Typing Lua commands one by one in the shell soon gets tedious. You can write more complex program in files, then execute them.

The Lua command `dofile` can be used to launch any script. For example, the following command will execute a script modifying the settings of the 3d window:

```
# Assuming the working directory is the same as the binary.
dofile("lua/settings.lua")
```

## 3d window usage

The window must have the focus before doing anything with it.

The camera can be moved using the WASD keys, and "Left Ctrl" and "Space" for vertical translation.

To turn the camera around, first press the Q key to lock the mouse, then just move the mouse around. Press Q again to unlock the mouse.

Bindings can be modified from the terminal: [azerty.lua](run/lua/bindings/azerty.lua) or [qwerty.lua](run/lua/bindings/qwerty.lua) are provided as examples.
Settings like mouse sensitivity or camera speed can be modified the same way: see [settings.lua](run/lua/settings.lua) for example.

# First Lua scripts

## Physics

The main instance of the [World](src/physics/README.md#World-class) class can be found at `insight.world` in Lua scripts.

New bodies can be created using the `insight.world:newBody()` method. It takes a [Shape](https://github.com/vsaulue/Insight/blob/readme/src/physics/README.md#shape-class--derived) object as argument. See [demoShapes.lua](run/lua/demos/demoShapes.lua) for a list of all shapes.

```
sphere = insight.world:newBody({
    shape= {
        type= "Sphere",
        params= {
            radius= 0.3, -- m
            density= 1000, -- kg/m^3
        },
    },
})
```

The return value is the created [Body](src/physics/README.md#Body-class). It can be used to move it in the world:

```
sphere:setPosition({0,5,0}) -- Move the sphere upwards
sphere:setVelocity({-1,0,0}) -- Gives horizontal velocity
```

See the [physics Readme](src/physics/README.md) for a more details.

## Robotics

New robots can be created as follows:

```
newRobot = insight:newRobot(constructionInfo,aiInfo)
```

Where:

* `constructionInfo` is an object describing the body of the robot.
* `aiInfo` is an object describing the control programs.

The `constructionInfo` part can be constructed from a Lua table with the following fields:

* parts: a table of [shapes](src/physics/README.md#shape-class--derived) indexed by names.
* basePart: name of the "reference" part (used for easier manipulation).
* joints: a table of [JointInfos](src/robotics/README.md#jointinfo-class--derived) indexed by names. Currently, 1 or 3 degree of freedom (dof) joints are supported. They can optionally include friction/damping, and motors.

Examples of `constructionInfo` tables are provided in the following scripts:

* [ballBotInfo](run/lua/robots/ballBotInfo.lua), a simple robot with a single 3-dof joint.
* [pendulumInfo](run/lua/robots/pendulumInfo.lua), a pendulum with a single 1-dof joint.
* [androidInfo](run/lua/robots/androidInfo.lua) for a more complex example.

Here's a full sample code to create a new android:

```
androidInfo = require("robots/androidInfo")
newAndroid = insight:newRobot(androidInfo, {type="feedback"})

-- Move the "basePart" to the designated coordinates, and all other parts of the robot to keep
-- the relative position of each parts.
newAndroid:setPosition({0,0,5})
```

## AIs

Currently, AIs (or control laws) can't be implemented from Lua. An extremely basic control
law is provided as a placeholder, named [feedback](src/AIs/README.md#feedbackai). It can be instanciated like this:

```
newAndroid = insight:newRobot(androidInfo, {type="feedback"})
```

Future versions will include ways to program AIs/control loops at runtime.

# Compiling

Platform: Windows 64 bits
Compiler: g++ 7.3.0, posix threads, seh exceptions (from MinGW-W64-builds-4.3.4).
CMake: v3.9 or higher

3-rd party libraries (thanks to all the contributors of these project):

* [Bullet physics 3](https://github.com/bulletphysics/bullet3) v2.88
* [Boost](https://www.boost.org/) v1.61
* [Irrlicht](http://irrlicht.sourceforge.net/) v1.8.3
* [lua53](https://www.lua.org/) v5.3.4 (compiled in c++)
* [catch](https://github.com/catchorg/Catch2) v1.3.4 (unit tests only), included in this repository.

While the 3rd party libraries & this code should be portable to other platforms (linux/macOs) or other compilers, this has never been attempted before (so unlikely to work without some tweaking).

If you want to build from the sources, or contribute to this project, feel free to contact me for help.

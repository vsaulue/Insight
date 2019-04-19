# Insight - AI experiment platform

This project provides tools to test 3d robot control programs. These programs can be implemented by manually programming robotic control laws, reinforcement learning...

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

# Writing Lua scripts

@TODO

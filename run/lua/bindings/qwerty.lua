-- Defines input bindings for QWERTY keyboards

-- Bindings (which key triggers which action) can be read/redefined from the
-- variable insight.graphicEngine.inputs.bindings. It has the following methods
-- * listActions(): list all action names.
-- * listEvents(): list all events names. They can correspond to a keyboard key, a mouse movement, ...
-- * get(actionName) : get the event name associated to action name.
-- * set(actionName, eventName) : replace actionName triggering event with eventName.

local QWERTY_BINDINGS = {
    -- Move camera
    CameraForward="KEY_W",
    CameraBackward="KEY_S",
    CameraLeft="KEY_A",
    CameraRight="KEY_D",
    CameraUp="KEY_Space",
    CameraDown="KEY_LeftCtrl",
    -- Turn on/off mouse lock in the middle of the screen. Needed for Mouse_Move events.
    ToggleMouseLock="KEY_Q",
    -- Turn the camera
    CameraTurnLeft="MOUSE_MoveLeft",
    CameraTurnRight="MOUSE_MoveRight",
    CameraTurnUp="MOUSE_MoveUp",
    CameraTurnDown="MOUSE_MoveDown"
}

for action,input in pairs(QWERTY_BINDINGS) do
    insight.graphicEngine.inputs.bindings:set(action, input)
end
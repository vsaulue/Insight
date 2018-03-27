-- Example of script defining custom input settings.

-- Input settings can be read/redefined from the variable insight.graphicEngine.inputs.settings.
-- It has the following methods:
-- * list(): list all settings names.
-- * get(settingsName) : get the value of a settings.
-- * set(actionName, number) : set the value of a settings (number is a float).

local settings = insight.graphicEngine.inputs.settings
settings:set("CameraTranslationSpeed", 5.0)
settings:set("CameraRotationSpeed", 27.5)
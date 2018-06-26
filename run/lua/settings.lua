-- Example of script defining custom input settings.

-- Input settings can be read/redefined from the variable insight.graphicEngine.inputs.settings.
-- It has the following methods:
-- * list(): list all settings names.
-- Each setting name is a valid field of the object, which has the following fields:
-- * value: raw numerical value of this setting.
-- * unit: unit of this setting.
-- * setValue: method used to change the raw value the setting.

local settings = insight.graphicEngine.inputs.settings
settings.CameraTranslationSpeed:setValue(5.0)
settings.CameraRotationSpeed:setValue(0.45)
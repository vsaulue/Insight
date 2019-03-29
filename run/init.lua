dofile(insight.dir .. "/lua/bindings/azerty.lua")

demos = require("demos/allDemos")

print("This is the default lua script, used to browse into some demo scripts.")
print("")
print("Available demonstrations:")
for name,demo in pairs(demos) do
    print("- " .. tostring(demo))
end

print("")
print("Type 'demos.<demoName>()' to launch a demo (ex: 'demo.physics()' ).")
print("")


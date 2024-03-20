# Auto Splitters

* Auto splitters are scripts that automate the process of timing your run in a game by making splits automatically, starting, resetting, pausing, etc.

# How do they work?

* These work by reading into game's memory and determining when the timer should do something, like make a split.

* LAST's autosplitting system works in a very similar way to LiveSplit's. The main difference is that LAST uses Lua instead of C#. There are also some key differences:
    * Runs an entire Lua system instead of only supporting specifically named C# blocks.
        * This means you can run external functions outside of the ones LAST executes.
    * Support for the entire Lua language, including the importing of libraries for tasks such as performance monitoring.

# How to make LAST auto splitters 

* It's somewhat easy if you know what you are doing or are porting an already existing one.

* First in the lua script goes a `process` function call with the name of the games process:

```lua
process('GameBlaBlaBla.exe')
```
* With this line, LAST will repeatedly attempt to find this process and will not continue script execution until it is found.

* Next we have to define the basic functions. Not all are required and the ones that are required may change depending on the game or end goal, like if loading screens are included or not.
    * The order at which these run is the same as they are documented below.

### `startup`
 The purpose of this function is to specify how many times LAST checks memory values and executes functions each second, the default is 60Hz. Usually, 60Hz is fine and this function can remain undefined. However, it's there if you need it.
```lua
process('GameBlaBlaBla.exe')

function startup()
    refreshRate = 120
end
```

### `state`
 The main purpose of this function is to assign memory values to Lua variables.
* Runs every 1000 / `refreshRate` milliseconds and when the script is enabled/loaded.

```lua
process('GameBlaBlaBla.exe')

local isLoading = false;

function startup()
    refreshRate = 120
end

function state()
    isLoading = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0);
end
```

* You may have noticed that we're assigning this `isLoading` variable with the result of the function `readAddress`. This function is part of LAST's Lua context and its purpose is to read memory values. It's explained in detail at the bottom of this document.

### `update`
 The purpose of this function is to update local variables.
* Runs every 1000 / `refreshRate` milliseconds.
```lua
process('GameBlaBlaBla.exe')

local current = {isLoading = false};
local old = {isLoading = false};
local loadCount = 0

function startup()
    refreshRate = 120
end

function state()
    old.isLoading = current.isLoading;

    current.isLoading = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0);
end

function update()
    if not current.isLoading and old.isLoading then
        loadCount = loadCount + 1;
    end
end
```
* We now have 3 variables, one represents the current state while the other the old state of isLoading, we also have loadCount getting updated in the `update` function which will store how many times we've entered the loading screen

### `start`
This tells LAST when to start the timer.\
_Note: LAST will ignore any start calls if the timer is running._
* Runs every 1000 / `refreshRate` milliseconds.
```lua
process('GameBlaBlaBla.exe')

local current = {isLoading = false};
local old = {isLoading = false};
local loadCount = 0

function startup()
    refreshRate = 120
end

function state()
    old.isLoading = current.isLoading;

    current.isLoading = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0);
end

function update()
    if not current.isLoading and old.isLoading then
        loadCount = loadCount + 1;
    end
end

function start()
    return current.isLoading
end
```

### `split`
Tells LAST to execute a split whenever it gets a true return.
    * Runs every 1000 / `refreshRate` milliseconds.
```lua
process('GameBlaBlaBla.exe')

local current = {isLoading = false};
local old = {isLoading = false};
local loadCount = 0

function startup()
    refreshRate = 120
end

function state()
    old.isLoading = current.isLoading;

    current.isLoading = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0);
end

function update()
    if not current.isLoading and old.isLoading then
        loadCount = loadCount + 1;
    end
end

function start()
    return current.isLoading
end

function split()
    local shouldSplit = false;
    if current.isLoading and not old.isLoading then
        loadCount = loadCount + 1;
        shouldSplit = loadCount > 1;
    end

    return shouldSplit;
end
```
* Whoa lots of code, why didnt we just return if we are currently in a loading screen like in start? Because if we do, we will do multiple splits a second, the function runs multiple times and it would do lots of unwanted splits.
* To solve that, we only want to split when we enter a loading screen (old is false, current is true), but we also don't want to split on the first loading screen as we have the assumption that the first loading screen is when the run starts. So that's where our loadCount comes in handy, we can just check if we are on the first one and only split when we aren't.

### `isLoading`
Pauses the timer whenever true is being returned.
* Runs every 1000 / `refreshRate` milliseconds.
```lua
process('GameBlaBlaBla.exe')

local current = {isLoading = false, scene = ""};
local old = {isLoading = false, scene = ""};
local loadCount = 0

function startup()
    refreshRate = 120
end

function state()
    old.isLoading = current.isLoading;
    old.scene = current.scene;

    current.isLoading = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0);
    current.scene = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xBB, 0xEE, 0x55, 0xDD, 0xBA, 0x6A);
end

function update()
    if not current.isLoading and old.isLoading then
        loadCount = loadCount + 1;
    end
end

function start()
    return current.isLoading
end

function split()
    local shouldSplit = false;
    if current.isLoading and not old.isLoading then
        loadCount = loadCount + 1;
        shouldSplit = loadCount > 1;
    end

    return shouldSplit;
end

function isLoading()
    return current.isLoading
end
```
* Pretty self explanatory, since we want to return whenever we are currently in a loading screen, we can just send our current isLoading status, same as start.

# `reset`
Instantly resets the timer. Use with caution.
* Runs every 1000 / `refreshRate` milliseconds.
```lua
process('GameBlaBlaBla.exe')

local current = {isLoading = false};
local old = {isLoading = false};
local loadCount = 0
local didReset = false

function startup()
    refreshRate = 120
end

function state()
    old.isLoading = current.isLoading;

    current.isLoading = readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0);
end

function update()
    if not current.isLoading and old.isLoading then
        loadCount = loadCount + 1;
    end
end

function start()
    return current.isLoading
end

function split()
    local shouldSplit = false;
    if current.isLoading and not old.isLoading then
        loadCount = loadCount + 1;
        shouldSplit = loadCount > 1;
    end

    return shouldSplit;
end

function isLoading()
    return current.isLoading
end

function reset()
    if not old.scene == "MenuScene" and current.scene == "MenuScene" then
        return true
    end
    return false
end
```
* In this example we are checking for the scene, of course, the address is completely arbitrary and doesnt mean anything for this example. Specifically we are checking if we are entering the MenuScene scene.

## readAddress
* `readAddress` is the second function that LAST defines for us and its globally available, its job is to read the memory value of a specified address.
* The first value defines what kind of value we will read:
    1. `sbyte`: signed 8 bit integer
    2. `byte`: unsigned 8 bit integer
    3. `short`: signed 16 bit integer
    4. `ushort`: unsigned 16 bit integer
    5. `int`: signed 32 bit integer
    6. `uint`: unsigned 32 bit integer
    7. `long`: signed 64 bit integer
    8. `ulong`: unsigned 64 bit integer
    9. `float`: 32 bit floating point number
    10. `double`: 64 bit floating point number
    11. `bool`: Boolean (true or false)
    12. `stringX`, A string of characters. Its usage is different compared the rest, you type "stringX" where the X is how long the string can be plus 1, this is to allocate the NULL terminator which defines when the string ends, for example, if the longest possible string to return is "cheese", you would define it as "string7". Setting X lower can result in the string terminating incorrectly and getting an incorrect result, setting it higher doesnt have any difference (aside from wasting memory).

* The second argument can be 2 things, a string or a number.
    * If its a number: The value in that memory address of the main process will be used.
    * If its a string: It will find the corresponding map of that string, for example "UnityPlayer.dll", This means that instead of reading the memory of the main map of the process (main binary .exe), it will instead read the memory of UnityPlayer.dll's memory space.
        * Next you have to add another argument, this will be the offset at which to read from from the perspective of the base address of the module, meaning if the module is mapped to 0x1000 to 0xFFFF and you put 0x0100 in the offset, it will read the value in the address 0x1010.

* The rest of arguments are memory offsets or pointer paths.
    * A Pointer Path is a list of Offsets + a Base Address. The auto splitter reads the value at the base address and interprets the value as yet another address. It adds the first offset to this address and reads the value of the calculated address. It does this over and over until there are no more offsets. At that point, it has found the value it was searching for. This resembles the way objects are stored in memory. Every object has a clearly defined layout where each variable has a consistent offset within the object, so you basically follow these variables from object to object.

        * Cheat Engine is a tool that allows you to easily find Addresses and Pointer Paths for those Addresses, so you don't need to debug the game to figure out the structure of the memory.

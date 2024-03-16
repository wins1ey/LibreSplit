# Auto Splitters

* Auto splitters are scripts that automate the process of timing your run in a game, this being making splits automatically, starting, resetting, pausing, etc.

# How do they work?

* These work by reading into game's memory (dont worry, you can't directly write to it) and determining when the timer should do something, like a split

* Auto splitters work in a very similar way to LiveSplit's ASL. The main difference is that LAST's use lua instead of C#. There are also some key differences:
    * Uses lua instead of named C# blocks
        * This means you can have more functions than the one LAST runs, of course you would be the one to call these as LAST wont run them
    * Incredibly modular
        * Because the whole lua script gets executed, you have at your disposal the entire programming language, this includes third-party libraries either for logging, performance profiling, filesystem, automation, etc

# I want one now, how do i make one?

* Its somewhat easy if you know what you are doing or are porting an already existing one

* First in the lua script goes a `process` function call with the name of the game's process:

```lua
process('GameBlaBlaBla.exe')
```
* Just with this, LAST will try to find the game's process when the auto splitter is running, nothing after this statement will get executed until the process is found

* Note: from now on we will append our stuff to this script and end up with a "working" script, it just doesnt belong to any game only does arbitrary stuff

* Next we have to define the basic functions, not all are required and the ones required may depend by game or rules, like if loading screens are included or not
    * The order at which these run is the same as they are documented below

### `startup`
 This function main usage is to set the refresh rate at which the functions run, the default is 60Hz, and the maximum is determined by your PC's speed and the game, it gets used like this, and it will set a refresh rate of 120Hz
* Runs once when the auto-splitter is enabled/loaded
* May also be used to initialize other variables in the script
```lua
process('GameBlaBlaBla.exe')

function startup()
    refreshRate = 120
end
```

### `state`
 This function main usage is to update a variable with whatever memory region you want from the game, either being a loading indicator, level counter, etc. Of course, you can do more stuff than that, its lua after all and you can do anything anywhere as long as its feasible
* Runs every 1000 / `refreshRate` milliseconds and when the script is enabled/loaded

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

* First of all, you can see on the third line we have decalred a variable named `isLoading` and we have given it the value `false`
* Then in the state function we are redefining it to the return value of `readAddress("bool", "UnityPlayer.dll", 0x019B4878, 0xD0, 0x8, 0x60, 0xA0, 0x18, 0xA0)`, `readAddress` is a function that LAST defines to the lua script, it is used to read a value of memory in the game in a specified address, Read [here](#readaddress) for documentation of this function.
* Great! now we have a script that check the value of a game's memory 120 times a second, but we arent doing anything with it yet...

### `update`
 This function main purpose is to update our own variables, we can keep track of how many times the game as entered a loading screen or do something completely else, really up to you
* Runs every 1000 / `refreshRate` milliseconds
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
* A lot has changed, we now have 3 variables, one represents the current state while the other the old state of isLoading, we also loadCount which is where we will store the amounts we entered the loading screen
* The update function was also added, we are incrementing the value in loadCount by 1 everytime we leave a loading screen. we are now sucesfully keeping track of the loading screens :D

### `start`
In this function we tell the emulator when to start a run, this only really matters when the timer is not running (0 seconds). Whenever we return true the timer will start, once the timer is running its return value doesnt matter
* Runs every 1000 / `refreshRate` milliseconds
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
* Looks simple enough, however this is with many assumptions, like that the game has a loading screen when starting a new game so that the run will start the moment the loading screen appears

### `split`
Whenever the split function returns true, we will do a split. In our case we will make a split on every loading screen
    * Runs every 1000 / `refreshRate` milliseconds
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
* To solve that, we only want to split when we enter a loading screen (old is false, current is true), But also we dont want to split on the first loading screen as we have done the assumption that the first loading screen is when the run starts, thats where our loadCount comes in handy, we can just check if we are on the first one and only split when we arent

### `isLoading`
As long as this function returns true, the timer will be paused, if the function doesnt exists the timer will never pause.
* Runs every 1000 / `refreshRate` milliseconds
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
* Pretty self explanatory, since we want to return whenever we are currently in a loading screen, we can just send our current isLoading status, same as start

# `reset`
It instantly resets the run, completly ignoring the attempt, use with caution to avoid unwanted reset, for example like going into the main menu
* Runs every 1000 / `refreshRate` milliseconds
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
* In this example we are checking for the scene, of course, the address is completely arbitrary and doesnt mean anything for this example. Specifically we are checking if we are entering the MenuScene scene

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
    12. `stringX`, A string of characters, Its usage is special than the rest, you type "stringX" where the X is how long the string can be plus 1, this is to allocate the NULL terminator which defines when the string ends, for example, if the longest possible string to return is "cheese", you would define it as "string7". Setting X lower can result in the string terminating incorrectly and being the result you didnt want, setting it higher doesnt have any difference

* The second argument can be 2 things, a string or a number
    * If its a number: The value in that memory address of the main process will be used
    * If its a string, it will find the corresponding map of that string, for example "UnityPlayer.dll", This means that instead of reading the memory of the main map of the process (main binary .exe), it will instead read the memory of UnityPlayer.dll's memory space.
        * Next you have to add another argument, this will be the offset at which to read from from the perspective of the base address of the module, meaning if the module is mapped to 0x1000 to 0xFFFF and you put 0x0100 in the offset, it will read the value in the address 0x1010

* The rest of arguments are memory offsets or pointer paths
    * A Pointer Path is a list of Offsets + a Base Address. The Auto Splitter reads the value at the base address and interprets the value as yet another address. It adds the first offset to this address and reads the value of the calculated address. It does this over and over until there are no more offsets. At that point, it has found the value it was searching for. This resembles the way objects are stored in memory. Every object has a clearly defined layout where each variable has a consistent offset within the object, so you basically follow these variables from object to object.

        * Cheat Engine is a tool that allows you to easily find Addresses and Pointer Paths for those Addresses, so you don't need to debug the game to figure out the structure of the memory.
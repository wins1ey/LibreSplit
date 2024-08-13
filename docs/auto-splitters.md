# Auto Splitters

* Auto splitters are scripts that automate the process of timing your run in a game by making splits automatically, starting, resetting, pausing, etc.

# How do they work?

* These work by reading into game's memory and determining when the timer should do something, like make a split.

* LibreSplit's autosplitting system works in a very similar way to LiveSplit's. The main difference is that LibreSplit uses Lua instead of C#. There are also some key differences:
    * Runs an entire Lua system instead of only supporting specifically named C# blocks.
        * This means you can run external functions outside of the ones LibreSplit executes.
    * Support for the entire Lua language, including the importing of libraries for tasks such as performance monitoring.

# Documentation

## `State`
* First in the lua script goes a State table where you will define an executable(s) and their addresses:

```lua
State = {
	Quake_x64_steam = {
		map = { "string255", "0x18DDE30" },
		intermission = { "int", "0x9DD3AEC" },
		menu = { "int", "0xE7AC84" },
	},
}
```
To define an executable, you create a table inside the block with the name of the executable.\
You then define your variables and assign addresses to them. The first field is where you define the variable type.\
You can use any of these types:
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
12. `stringX`, A string of characters. Its usage is different compared the rest, you type "stringX" where the X is how long the string can be plus 1, this is to allocate the NULL terminator which defines when the string ends, for example, if the longest possible string to return is "cheese", you would define it as "string7". Setting X lower can result in the string terminating incorrectly and getting an incorrect result, setting it higher doesnt have any difference (aside from wasting memory).\
\
You then define the address and its offsets (seperate each with a comma.)
```lua
variable = { "type", "address", "offset1", "offset2"}
```

## `Startup`
 The purpose of this function is to specify how many times LibreSplit checks memory values and executes functions each second, the default is 60Hz. Usually, 60Hz is fine and this function can remain undefined. However, it's there if you need it.
```lua
function Startup()
    RefreshRate = 120
end
```


## `Update`
 The purpose of this function is to perform whatever actions you want on each cycle.
 This function is most commonly used for updating local variables and debugging purposes.
* Runs every 1000 / `RefreshRate` milliseconds.
```lua
function Update()
  print("Current map: " .. tostring(current.map))
end
```


## `Start`
This tells LibreSplit when to start the timer.\
_Note: LibreSplit will ignore any start calls if the timer is running._
* Runs every 1000 / `RefreshRate` milliseconds.
```lua
function Start()
	if not current.map then
		print("Warning: current.map is nil")
		return false
	end

	local startMaps = settings.episodeRun and vars.episodeStarts or vars.fullGameStarts
	if indexOf(startMaps, current.map) > 0 then
		vars.lastMap = current.map
		return true
	end
	return false
end
```

## `Split`
Tells LibreSplit to execute a split whenever it gets a true return.
    * Runs every 1000 / `RefreshRate` milliseconds.
```lua
function Split()
    if current.level_id > old.level_id then
        return true
    end
end
```
* A common pitfall with this function is the autosplitter splitting more times than it should. The solution to this is to make it so the function only returns true for one cycle.
* One of the best ways to do this is to compare your current variable with its old version.

## `IsLoading`
Pauses the timer whenever true is being returned.
* Runs every 1000 / `RefreshRate` milliseconds.
```lua
function IsLoading()
    return current.isLoading
end
```
* Pretty self explanatory, since we want to return whenever we are currently in a loading screen, we can just send our current isLoading status.

## `Reset`
Instantly resets the timer. Use with caution.
* Runs every 1000 / `refreshRate` milliseconds.
```lua
function Reset()
	if current.menu == 1 and (not current.map or #current.map == 0) then
		vars.lastVisitedMaps = {}
		return true
	end
	return false
end
```


## GetPID
* Returns the current PID

# Experimental stuff
## `MapsCacheCycles`
* The biggest bottleneck with reading memory is having to read every line of `/proc/pid/maps` and checking if that line is the corresponding module. This option allows you to set for how many cycles the cache of that file should be used. The cache is global so it gets reset every x number of cycles.
    * `0` (default): Disabled completely
    * `1`: Enabled for the current cycle
    * `2`: Enabled for the current cycle and the next one
    * `3`: Enabled for the current cycle and the 2 next ones
    * You get the idea

### Performance
* Every uncached map finding takes around 1ms (depends a lot on your ram and cpu)
* Every cached map finding takes around 100us

* Mainly useful for autosplitters that use a lot of memory read calls and the game the autosplitter is for has an uncapped game state update rate, where literally every millisecond matters.
* You define `MapsCacheCycles` in the startup function.
```lua
function Startup()
    RefreshRate = 60;
    MapsCacheCycles = 1;
end
```

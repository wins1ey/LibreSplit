#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>

#include <lua.hpp>

#include "headers/autosplitter.hpp"
#include "headers/autosplitter.h"
#include "headers/lastprint.hpp"
#include "headers/downloader.hpp"
#include "headers/readmem.hpp"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::sort;
using std::to_string;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::filesystem::exists;
using std::filesystem::is_empty;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::this_thread::sleep_for;
using std::atomic;

char autoSplitterFile[256];
string autoSplittersDirectory;
string chosenAutoSplitter;
int refreshRate = 60;
atomic<bool> usingAutoSplitter(true);
atomic<bool> callStart(false);
atomic<bool> callSplit(false);
atomic<bool> toggleLoading(false);
atomic<bool> callReset(false);
bool prevIsLoading;

void checkDirectories()
{
    // Get the path to the users directory
    string userDirectory = getpwuid(getuid())->pw_dir;
    string lastDirectory = userDirectory + "/.last";
    autoSplittersDirectory = lastDirectory + "/auto-splitters";
    string themesDirectory = lastDirectory + "/themes";

    // Make the LAST directory if it doesn't exist
    if (!exists(lastDirectory))
    {
        create_directory(lastDirectory);
    }
    // Make the autosplitters directory if it doesn't exist
    if (!exists(autoSplittersDirectory))
    {
        create_directory(autoSplittersDirectory);
    }

    if (!exists(themesDirectory))
    {
        create_directory(themesDirectory);
    }
}

void startup(lua_State* L)
{
    lua_getglobal(L, "startup");
    lua_pcall(L, 0, 0, 0);

    lua_getglobal(L, "refreshRate");
    bool refreshRateExists = lua_isnumber(L, -1);
    lua_pop(L, 1); // Remove 'refreshRate' from the stack

    if (refreshRateExists)
    {
        lua_getglobal(L, "refreshRate");
        refreshRate = lua_tointeger(L, -1);
        lua_pop(L, 1); // Remove 'refreshRate' from the stack
    }
}

void state(lua_State* L)
{
    lua_getglobal(L, "state");
    lua_pcall(L, 0, 0, 0);
}

void update(lua_State* L)
{
    lua_getglobal(L, "update");
    lua_pcall(L, 0, 0, 0);
}

void start(lua_State* L)
{
    lua_getglobal(L, "start");
    lua_pcall(L, 0, 1, 0);
    if (lua_toboolean(L, -1))
    {
        callStart.store(true);
    }
    lua_pop(L, 1); // Remove the return value from the stack
}

void split(lua_State* L)
{
    lua_getglobal(L, "split");
    lua_pcall(L, 0, 1, 0);
    if (lua_toboolean(L, -1))
    {
        callSplit.store(true);
    }
    lua_pop(L, 1); // Remove the return value from the stack
}

void isLoading(lua_State* L)
{
    lua_getglobal(L, "isLoading");
    lua_pcall(L, 0, 1, 0);
    if (lua_toboolean(L, -1) != prevIsLoading)
    {
        toggleLoading.store(true);
        prevIsLoading = !prevIsLoading;
    }
    else if (!lua_toboolean(L, -1) == prevIsLoading)
    {
        toggleLoading.store(true);
        prevIsLoading = !prevIsLoading;
    }
    lua_pop(L, 1); // Remove the return value from the stack
}

void reset(lua_State* L)
{
    lua_getglobal(L, "reset");
    lua_pcall(L, 0, 1, 0);
    if (lua_toboolean(L, -1))
    {
        callReset.store(true);
    }
    lua_pop(L, 1); // Remove the return value from the stack
}

void runAutoSplitter()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, findProcessID);
    lua_setglobal(L, "process");
    lua_pushcfunction(L, readAddress);
    lua_setglobal(L, "readAddress");
    lua_pushcfunction(L, luaPrint);
    lua_setglobal(L, "lastPrint");
    string currentAutoSplitterFile = autoSplitterFile;

    // Load the Lua file
    if (luaL_loadfile(L, autoSplitterFile) != LUA_OK)
    {
        // Error loading the file
        const char* errorMsg = lua_tostring(L, -1);
        lua_pop(L, 1); // Remove the error message from the stack
        throw std::runtime_error("Lua syntax error: " + std::string(errorMsg));
    }

    // Execute the Lua file
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
    {
        // Error executing the file
        const char* errorMsg = lua_tostring(L, -1);
        lua_pop(L, 1); // Remove the error message from the stack
        throw std::runtime_error("Lua runtime error: " + std::string(errorMsg));
    }

    lua_getglobal(L, "state");
    bool stateExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'state' from the stack
    
    lua_getglobal(L, "start");
    bool startExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'start' from the stack
    
    lua_getglobal(L, "split");
    bool splitExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'split' from the stack

    lua_getglobal(L, "isLoading");
    bool isLoadingExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'isLoading' from the stack

    lua_getglobal(L, "startup");
    bool startupExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'startup' from the stack

    lua_getglobal(L, "reset");
    bool resetExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'reset' from the stack

    lua_getglobal(L, "update");
    bool updateExists = lua_isfunction(L, -1);
    lua_pop(L, 1); // Remove 'update' from the stack

    if (startupExists)
    {
        startup(L);
    }

    if (stateExists)
    {
        state(L);
    }

    lastPrint("Refresh rate: " + to_string(refreshRate));
    int rate = static_cast<int>(1000000 / refreshRate);

    while (usingAutoSplitter.load() && currentAutoSplitterFile == autoSplitterFile)
    {
        auto clockStart = high_resolution_clock::now();

        if (stateExists)
        {
            state(L);
        }

        if (updateExists)
        {
            update(L);
        }
            
        if (startExists)
        {
            start(L);
        }

        if (splitExists)
        {
            split(L);
        }

        if (isLoadingExists)
        {
            isLoading(L);
        }

        if (resetExists)
        {
            reset(L);
        }

        auto clockEnd = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(clockEnd - clockStart).count();
        if (duration < rate)
        {
            sleep_for(microseconds(rate - duration));
        }
    }
    
    atomic_store(&usingAutoSplitter, false);
    lua_close(L);
    openAutoSplitter();
}

void openAutoSplitter()
{
    while (true)
    {
        if (usingAutoSplitter.load() && autoSplitterFile[0] != '\0')
        {
            runAutoSplitter();
        }
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for 100 milliseconds before checking again
    }
}
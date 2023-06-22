#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <filesystem>

#include <lua.hpp>

#include "include/autosplitter.hpp"
#include "include/lasprint.hpp"
#include "include/downloader.hpp"
#include "include/client.hpp"
#include "include/readmem.hpp"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::filesystem::exists;
using std::filesystem::is_empty;

lua_State* L = luaL_newstate();

string autoSplittersDirectory;
string chosenAutoSplitter;
vector<string> fileNames;

void checkDirectories()
{
    string executablePath;
    string executableDirectory;

    // Get the path to the executable
    char result[ PATH_MAX ];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    executablePath = string(result, (count > 0) ? count : 0);
    executableDirectory = executablePath.substr(0, executablePath.find_last_of("/"));

    autoSplittersDirectory = executableDirectory + "/autosplitters";

    // Make the autosplitters directory if it doesn't exist
    if (!exists(autoSplittersDirectory))
    {
        create_directory(autoSplittersDirectory);
    }
}

void chooseAutoSplitter()
{
    if (is_empty(autoSplittersDirectory))
    {
        startDownloader(autoSplittersDirectory);
    }
    lasPrint("clear");
    lasPrint("Auto Splitter: ");
    cout << endl;
    int counter = 1;
    for (const auto & entry : directory_iterator(autoSplittersDirectory))
    {
        if (entry.path().extension() == ".lua")
        {
            cout << counter << ". " << entry.path().filename() << endl;
            fileNames.push_back(entry.path().string());
            counter++;
        }
    }

    switch (fileNames.size())
    {
        case 0:
        {
            cout << "No auto splitters found. Please put your auto splitters in the autosplitters folder or download some here.\n";
            startDownloader(autoSplittersDirectory);
            chooseAutoSplitter();
            break;
        }
        case 1:
        {
            chosenAutoSplitter = fileNames[0];
            break;
        }
        default:
        {
            int userChoice;
            cout << "Which auto splitter would you like to use? ";
            cin >> userChoice;
            cin.ignore();
            chosenAutoSplitter = fileNames[userChoice - 1];
            break;
        }
    }
    lasPrint(chosenAutoSplitter.substr(chosenAutoSplitter.find_last_of("/") + 1) + "\n");
}

void runAutoSplitter()
{
    luaL_openlibs(L);
    lua_pushcfunction(L, findProcessID);
    lua_setglobal(L, "process");
    lua_pushcfunction(L, readAddress);
    lua_setglobal(L, "readAddress");
    lua_pushcfunction(L, sendCommand);
    lua_setglobal(L, "sendCommand");
    lua_pushcfunction(L, luaPrint);
    lua_setglobal(L, "lasPrint");

    luaL_dofile(L, chosenAutoSplitter.c_str());
    lua_close(L);
}
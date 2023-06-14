#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <filesystem>

#include <lua.hpp>

#include "client.h"
#include "readmem.h"
#include "downloader.h"
#include "lasprint.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::exception;
using std::cerr;
using std::runtime_error;
using std::getline;
using std::cin;
using std::filesystem::path;
using std::filesystem::directory_iterator;
using std::filesystem::create_directory;
using std::filesystem::exists;

lua_State* L = luaL_newstate();

string autoSplittersDirectory;
string chosenAutoSplitter;
vector<string> fileNames;

int sendCommand(lua_State* L)
{
    try
    {
        sendLSCommand(lua_tostring(L, 1));
    }
    catch (const exception& e)
    {
        cerr << "\033[1;31m" << e.what() << endl << endl;
        throw;
    }

    return 0;
}

void checkDirectories()
{
    string executablePath;
    string executableDirectory;

    // Get the path to the executable
    char result[ PATH_MAX ];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    executablePath = string(result, (count > 0) ? count : 0);
    executableDirectory = executablePath.substr(0, executablePath.find_last_of("/"));

    autoSplittersDirectory = executableDirectory + "/autosplitters/";

    // Make the autosplitters directory if it doesn't exist
    if (!exists(autoSplittersDirectory))
    {
        create_directory(autoSplittersDirectory);
        startDownloader(autoSplittersDirectory);
    }


}

void chooseAutoSplitter()
{
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

void setIpAddress()
{
    string ipAddress;
    cout << "What is your local IP address? (Leave blank for 127.0.0.1)\n";
    getline(cin, ipAddress);
    if (ipAddress.empty()) {
        ipAddress = "127.0.0.1";
    }
    try
    {
        Client(ipAddress);
    }
    catch (const exception& e)
    {
        cerr << "\n\033[1;31m" << e.what() << endl << endl;
        throw;
    }
}

void runAutoSplitter()
{
    luaL_openlibs(L);
    lua_pushcfunction(L, processID);
    lua_setglobal(L, "processID");
    lua_pushcfunction(L, readAddress);
    lua_setglobal(L, "readAddress");
    lua_pushcfunction(L, sendCommand);
    lua_setglobal(L, "sendCommand");
    lua_pushcfunction(L, luaPrint);
    lua_setglobal(L, "lasPrint");

    luaL_dofile(L, chosenAutoSplitter.c_str());
    lua_close(L);
}

int main(int argc, char *argv[])
{
    checkDirectories();

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-downloader") == 0)
        {
            startDownloader(autoSplittersDirectory);
        }
    }

    chooseAutoSplitter();
    setIpAddress();
    runAutoSplitter();

    return 0;
}
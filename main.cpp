#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/uio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <thread>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <lua.hpp>

#include "readmem.hpp"
#include "client.hpp"
#include "downloader.hpp"

using namespace std;

ReadMemory readMemory;
LiveSplitClient lsClient;
Downloader downloader;

string chosenAutoSplitter;
string ipAddress;
string processName;

int pid = 0;
uint32_t memValue;

struct iovec valueLocal;
struct iovec valueRemote;

struct StockPid
{
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;

void Func_StockPid(const char *processtarget)
{
    stockthepid.pid_pipe = popen(processtarget, "r");
    if (!fgets(stockthepid.buff, 512, stockthepid.pid_pipe))
    {
        cout << "Error reading process ID: " << strerror(errno) << endl;
    }

    stockthepid.pid = strtoul(stockthepid.buff, nullptr, 10);

    if (stockthepid.pid != 0)
    {
        cout << processName + " is running - PID NUMBER -> " << stockthepid.pid << endl;
        pclose(stockthepid.pid_pipe);
        pid = stockthepid.pid;
        lsClient.Client(pid, ipAddress);
    }
    else {
        pclose(stockthepid.pid_pipe);
    }
}

int processID(lua_State* L)
{
    processName = lua_tostring(L, 1);
    string command = "pidof " + processName;
    const char *cCommand = command.c_str();

    Func_StockPid(cCommand);
    while (pid == 0)
    {
        cout << processName + " isn't running. Retrying in 5 seconds...\n";
        sleep(5);
        system("clear");
        Func_StockPid(cCommand);
    }

    return 0;
}

int readAddress(lua_State* L)
{
    uint64_t address = lua_tointeger(L, 1) + lua_tointeger(L, 2);
    uint32_t value = readMemory.readMem(memValue, pid, address, valueLocal, valueRemote);
    lua_pushinteger(L, value);

    this_thread::sleep_for(chrono::microseconds(1));

    return 1;
}

int sendCommand(lua_State* L)
{
    lsClient.sendLSCommand(lua_tostring(L, 1));
    return 0;
}

void chooseAutoSplitter()
{
    string path = "autosplitters";
    vector<string> file_names;

    int counter = 1;
    for (const auto & entry : filesystem::directory_iterator(path))
    {
        if (entry.path().extension() == ".lua")
        {
        cout << counter << ". " << entry.path().filename() << endl;
        file_names.push_back(entry.path().string());
        counter++;
        }
    }

    if (file_names.size() == 1)
    {
        chosenAutoSplitter = file_names[0];
    }
    else if (file_names.size() > 1)
    {
        int userChoice;
        cout << "Which auto splitter would you like to use? (Enter the number) ";
        cin >> userChoice;
        chosenAutoSplitter = file_names[userChoice - 1];
    }
    else {
        cout << "No auto splitters found. Please put your auto splitters in the autosplitters folder or download some here.\n";
        downloader.startDownloader();
    }
    cout <<  chosenAutoSplitter << endl;
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-downloader") == 0)
        {
            downloader.startDownloader();
            return 0;
        }
    }

    chooseAutoSplitter();

    cin.ignore();
    cout << "What is your local IP address? (Leave blank for 127.0.0.1)\n";
    getline(cin, ipAddress);
    if (ipAddress.empty())
    {
        ipAddress = "127.0.0.1";
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, processID);
    lua_setglobal(L, "processID");
    lua_pushcfunction(L, readAddress);
    lua_setglobal(L, "readAddress");
    lua_pushcfunction(L, sendCommand);
    lua_setglobal(L, "sendCommand");

    luaL_dofile(L, chosenAutoSplitter.c_str());
    lua_close(L);

    return 0;
}
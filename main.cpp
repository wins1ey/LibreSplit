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

#include <lua.hpp>

#include "readmem.hpp"
#include "client.hpp"

using namespace std;

ReadMemory readMemory;
LiveSplitClient lsClient;

bool episode = false;
string ipAddress = "";
string processName;

int pid = 0;
uint64_t address;
uint32_t memValue;

struct iovec valueLocal;
struct iovec valueRemote;

struct StockPid
{
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;

int Func_StockPid(const char *processtarget)
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

    return 0;
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
    address = lua_tointeger(L, 1);
    uint32_t value = readMemory.readMem(memValue, pid, address, valueLocal, valueRemote);
    lua_pushinteger(L, value);

    return 1;
}

int startTimer(lua_State* L)
{
    lsClient.sendLSCommand("starttimer\r\n");
    return 0;
}

int pauseGameTime(lua_State* L)
{
    lsClient.sendLSCommand("pausegametime\r\n");
    return 0;
}

int unpauseGameTime(lua_State* L)
{
    lsClient.sendLSCommand("unpausegametime\r\n");
    return 0;
}

int split(lua_State* L)
{
    lsClient.sendLSCommand("split\r\n");
    return 0;
}

int main(int argc, char *argv[])
{

    cout << "What is your local IP address? (LiveSplit Server settings will tell you if you don't know.)\n";
    cin >> ipAddress;

    string path = "autosplitters";
    string chosenAutosplitter;
    vector<string> file_names;

    int counter = 1;
    for (const auto & entry : filesystem::directory_iterator(path))
    {
        cout << counter << ". " << entry.path().filename() << endl;
        file_names.push_back(entry.path().string());
        counter++;
    }

    if (file_names.size() == 1)
    {
        chosenAutosplitter = file_names[0];
    }
    else if (file_names.size() > 1)
    {
        int userChoice;
        cout << "Which auto splitter would you like to use? (Enter the number) ";
        cin >> userChoice;
        chosenAutosplitter = file_names[userChoice - 1];
    }
    else {
        cout << "No auto splitters found. Please put your auto splitters in the autosplitters folder and restart the program.\n";
        return 0;
    }
    cout <<  chosenAutosplitter << endl;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, processID);
    lua_setglobal(L, "processID");
    lua_pushcfunction(L, readAddress);
    lua_setglobal(L, "readAddress");
    lua_pushcfunction(L, startTimer);
    lua_setglobal(L, "startTimer");
    lua_pushcfunction(L, pauseGameTime);
    lua_setglobal(L, "pauseGameTime");
    lua_pushcfunction(L, unpauseGameTime);
    lua_setglobal(L, "unpauseGameTime");
    lua_pushcfunction(L, split);
    lua_setglobal(L, "split");

    luaL_dofile(L, chosenAutosplitter.c_str());
    lua_close(L);

    return 0;


/*
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-ep") == 0)
        {
            episode = true;
        }
    }
*/
}
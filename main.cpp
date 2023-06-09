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

int pid;
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

    if (stockthepid.pid == 0)
    {
        cout << processName + " isn't running.\n";
        pclose(stockthepid.pid_pipe);
    }
    else
    {
        cout << processName + " is running - PID NUMBER -> " << stockthepid.pid << endl;
        pclose(stockthepid.pid_pipe);
        pid = stockthepid.pid;
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

/*
int sendCommands()
{
    lsClient.Client(pid, ipAddress);

    uint32_t prevStart;
    uint32_t prevLoading;
    uint32_t prevMenuStage;
    uint32_t prevPaused;

    while(true)
    {
        //std::thread t1(readAddresses);
        //std::thread t2(readAddresses);

        //t1.join();
        //t2.join();

        // Autosplitter

        if (episode == true && (loading == 0 && prevLoading == 1))
        {
            lsClient.sendLSCommand("starttimer\r\n");
        }
        else if (start == 0 && prevStart == 2)
        {
            lsClient.sendLSCommand("starttimer\r\n");
        }

        if ((loading == 1 && prevLoading == 0) || (menuStage == 3 & paused == 4) && (prevMenuStage != 3 || prevPaused != 4))
        {
            lsClient.sendLSCommand("pausegametime\r\n");
        }
        else if ((loading != 1 && prevLoading == 1) && ((menuStage != 3 || paused != 4)))
        {
            lsClient.sendLSCommand("unpausegametime\r\n");
        }

        if ((menuStage == 3 && prevMenuStage == 2) && paused != 28 && paused != 3)
        {
            lsClient.sendLSCommand("split\r\n");
        }

        prevStart = start;
        prevLoading = loading;
        prevMenuStage = menuStage;
        prevPaused = paused;

        sleep(0.0001); // Sleep to avoid CPU explosio
    }

    return 0;
}
*/

int processID(lua_State* L)
{
    processName = lua_tostring(L, 1);
    string command = "pidof " + processName;
    const char *cCommand = command.c_str();
    cout << cCommand << endl;

    Func_StockPid(cCommand);

    return 0;
}

int main(int argc, char *argv[])
{

    cout << "What is your local IP address? (LiveSplit Server settings will tell you if you don't know.)\n";
    cin >> ipAddress;

    string path = "autosplitters";
    vector<string> file_names;
    int counter = 1;
    for (const auto & entry : filesystem::directory_iterator(path))
    {
        cout << counter << ". " << entry.path().filename() << endl;
        file_names.push_back(entry.path().string());
        counter++;
    }

    int userChoice;
    cout << "Which autosplitter would you like to use? (Enter the number) ";
    cin >> userChoice;
    string chosenAutosplitter = file_names[userChoice - 1];
    cout << "You chose " << chosenAutosplitter << endl;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, processID);
    lua_setglobal(L, "processID");
    lua_pushcfunction(L, readAddress);
    lua_setglobal(L, "readAddress");

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

    cout << "What is your local IP address? (LiveSplit Server settings will tell you if you don't know.)\n";
    cin >> ipAddress;

    const char *processName = "pgrep [A]midEvil-Win64";
    while (true)
    {
        Func_StockPid(processName);
        if (stockthepid.pid == 0)
        {
            cout << "AMID EVIL isn't running. Retrying in 5 seconds...\n";
            sleep(5);
            system("clear");
        }
        else
        {
            break;
        }
    }
    sendCommands(stockthepid.pid);
    return 0;

*/
}
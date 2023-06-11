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

using namespace std;

ReadMemory readMemory;
LiveSplitClient lsClient;

bool episode = false;
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

int downloadFile(string url)
{
    CURL *curl;
    CURLcode res;
    string filename = url.substr(url.find_last_of("/") + 1);
    string filepath = "autosplitters/" + filename;

    curl = curl_easy_init();
    if (curl)
    {
        FILE *fp = fopen(filepath.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    return 0;
}

int downloader()
{
    cout << "Auto Splitter Downloader\n";
    downloadFile("https://raw.githubusercontent.com/Wins1ey/AutoSplitters/main/downloadable.csv");

    ifstream file("autosplitters/downloadable.csv");
    string line;
    int i = 1;
    vector<string> gameNamesVector;
    vector<string> urlsVector;
    while (getline(file, line))
    {
        istringstream iss(line);
        string gameName;
        string url;
        getline(iss, gameName, ',');
        getline(iss, url, ',');
        cout << i << ". " + gameName + "\n";
        i++;
        gameNamesVector.push_back(gameName);
        urlsVector.push_back(url);
    }
    file.close();

    cout << "Which auto splitters would you like to download? (Numbers separated by spaces): ";
    vector<int> choiceOfAutoSplitters;
    int singleChoice;
    while (cin >> singleChoice)
    {
        choiceOfAutoSplitters.push_back(singleChoice);
        if (cin.peek() == '\n')
        {
            break;
        }
    }

    for (int i = 0; i < choiceOfAutoSplitters.size(); i++)
    {
        int singleChoice = choiceOfAutoSplitters[i];
        if (singleChoice > 0 && singleChoice <= gameNamesVector.size())
        {
            cout << "Downloading " + gameNamesVector[singleChoice - 1] + "'s auto splitter\n";
            downloadFile(urlsVector[singleChoice - 1]);
        }
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

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-downloader") == 0)
        {
            downloader();
            return 0;
        }
    }

    string path = "autosplitters";
    string chosenAutosplitter;
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

    luaL_dofile(L, chosenAutosplitter.c_str());
    lua_close(L);

    return 0;
}
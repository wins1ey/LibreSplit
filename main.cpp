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

#include "readmem.hpp"
#include "client.hpp"

using std::string;
using std::cout;
using std::endl;
using std::string;
using std::cin;

ReadMemory readMemory;
LiveSplitClient lsClient;

string ipAddress = "";

uint32_t start, loading, menuStage, paused;

uint32_t memStart;
uint32_t memLoading;
uint32_t memMenuStage;
uint32_t memPaused;

struct iovec startLocal;
struct iovec startRemote;

struct iovec loadingLocal;
struct iovec loadingRemote;

struct iovec menuStageLocal;
struct iovec menuStageRemote;

struct iovec pausedLocal;
struct iovec pausedRemote;

struct StockPid
{
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;

void Func_StockPid(const char *processtarget)
{
    stockthepid.pid_pipe = popen(processtarget, "r");
    if (!fgets(stockthepid.buff, 512, stockthepid.pid_pipe)) {
        cout << "Error reading process ID: " << strerror(errno) << endl;
    }

    stockthepid.pid = strtoul(stockthepid.buff, nullptr, 10);

    if (stockthepid.pid == 0) {
        cout << "Jet Set Radio isn't running.\n";
        pclose(stockthepid.pid_pipe);
    } else {
        cout << "Jet Set Radio is running - PID NUMBER -> " << stockthepid.pid << endl;
        pclose(stockthepid.pid_pipe);
    }
}

void readAddresses(int pid)
{
    start = readMemory.readMem(memStart, pid, 0x6BAFFD0, startLocal, startRemote);
    loading = readMemory.readMem(memLoading, pid, 0x6E76B0C, loadingLocal, loadingRemote);
    menuStage = readMemory.readMem(memMenuStage, pid, 0x6F75F14, menuStageLocal, menuStageRemote);
    paused = readMemory.readMem(memPaused, pid, 0x6B95A68, pausedLocal, pausedRemote);

}

void sendCommands(int pid)
{
    lsClient.Client(pid, ipAddress);

    uint32_t prevStart;
    uint32_t prevLoading;
    uint32_t prevMenuStage;
    uint32_t prevPaused;

    while(true)
    {
        std::thread t1(readAddresses, pid);
        std::thread t2(readAddresses, pid);

        t1.join();
        t2.join();
        


        sleep(0.0001); // Sleep to avoid CPU explosio
    }
}

int main(int argc, char *argv[]) {

    cout << "What is your local IP address? (LiveSplit Server settings will tell you if you don't know.)\n";
    cin >> ipAddress;

    const char *processName = "pidof AmidEvil-Win64-Shipping.exe";
    while (true) {
        Func_StockPid(processName);
        if (stockthepid.pid == 0) {
            cout << "AMID EVIL isn't running. Retrying in 5 seconds...\n";
            sleep(5);
            system("clear");
        } else {
            break;
        }
    }
    sendCommands(stockthepid.pid);
    return 0;
}
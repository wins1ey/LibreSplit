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

using namespace std;

string ipAddress = "";

uint32_t loading; //isLoading
uint32_t newGame; //start
uint32_t rankingScreen; //split
uint32_t bossGraffiti; //Additional condition

struct StockPid {
    pid_t pid;
    char buff[512];
    FILE *pid_pipe;
} stockthepid;

void Func_StockPid(const char *processtarget) {
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

void ReadProcessMemory(int pid, uint32_t &loading, uint32_t &newGame, uint32_t &rankingScreen, uint32_t &bossGraffiti) {
    //Loading
    struct iovec loadingLocal, loadingRemote;
    loadingLocal.iov_base = &loading;
    loadingLocal.iov_len = sizeof(loading);
    loadingRemote.iov_base = (void *) 0x98FAAC;
    loadingRemote.iov_len = sizeof(loading);

    ssize_t loadingnread = process_vm_readv(pid, &loadingLocal, 1, &loadingRemote, 1, 0);
    if(loadingnread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(loadingnread != loadingRemote.iov_len) {
        cout << "Error reading process memory: short read of " << loadingnread << " bytes" << endl;
        exit(-1);
    }

    //newGame
    struct iovec newGameLocal, newGameRemote;
    newGameLocal.iov_base = &newGame;
    newGameLocal.iov_len = sizeof(newGame);\
    newGameRemote.iov_base = (void *) 0xB5A278;
    newGameRemote.iov_len = sizeof(newGame);

    ssize_t newGamenread = process_vm_readv(pid, &newGameLocal, 1, &newGameRemote, 1, 0);
    if(newGamenread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(newGamenread != newGameRemote.iov_len) {
        cout << "Error reading process memory: short read of " << newGamenread << " bytes" << endl;
        exit(-1);
    }

    //rankingScreen
    struct iovec rankingScreenLocal, rankingScreenRemote;
    rankingScreenLocal.iov_base = &rankingScreen;
    rankingScreenLocal.iov_len = sizeof(rankingScreen);\
    rankingScreenRemote.iov_base = (void *) 0x98FB1C;
    rankingScreenRemote.iov_len = sizeof(rankingScreen);

    ssize_t rankingScreennread = process_vm_readv(pid, &rankingScreenLocal, 1, &rankingScreenRemote, 1, 0);
    if(rankingScreennread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(rankingScreennread != rankingScreenRemote.iov_len) {
        cout << "Error reading process memory: short read of " << rankingScreennread << " bytes" << endl;
        exit(-1);
    }

    //bossGraffiti
    struct iovec bossGraffitiLocal, bossGraffitiRemote;
    bossGraffitiLocal.iov_base = &bossGraffiti;
    bossGraffitiLocal.iov_len = sizeof(bossGraffiti);\
    bossGraffitiRemote.iov_base = (void *) 0x95D2B8;
    bossGraffitiRemote.iov_len = sizeof(bossGraffiti);

    ssize_t bossGraffitinread = process_vm_readv(pid, &bossGraffitiLocal, 1, &bossGraffitiRemote, 1, 0);
    if(bossGraffitinread == -1) {
        cout << "Error reading process memory: " << strerror(errno) << endl;
        exit(-1);
    } else if(bossGraffitinread != bossGraffitiRemote.iov_len) {
        cout << "Error reading process memory: short read of " << bossGraffitinread << " bytes" << endl;
        exit(-1);
    }
}

void Client(int pid, string ipAddress, uint32_t& loading, uint32_t& newGame, uint32_t& rankingScreen, uint32_t& bossGraffiti) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cout << "Socket error: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    int port = 16834;

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) != 1) {
        cout << "Invalid IP address" << endl;
        exit(EXIT_FAILURE);
    }

    int connectRes = connect(sock, (struct sockaddr*) &hint, sizeof(hint));
    if (connectRes == -1) {
        cout << "Couldn't connect: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    const char* pausegametime = "pausegametime\r\n";
    const char* unpausegametime = "unpausegametime\r\n";
    const char* initgametime = "initgametime\r\n";
    const char* starttimer = "starttimer\r\n";
    const char* split = "split\r\n";

    int sendRes = send(sock, initgametime, strlen(initgametime), 0);
    if (sendRes == -1) {
        cout << "Couldn't send initgametime: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    uint32_t prevLoading = 0;
    uint32_t prevNewGame = 0;
    uint32_t prevRankingScreen = 0;
    uint32_t prevBossGraffiti = 0;
    
    while (true) {
        ReadProcessMemory(pid, loading, newGame, rankingScreen, bossGraffiti);

        if (loading == 1 && prevLoading != 1) {
            send(sock, pausegametime, strlen(pausegametime), 0);
        } else if (loading == 0 && prevLoading != 0) {
            send(sock, unpausegametime, strlen(unpausegametime), 0);
        }
        prevLoading = loading;

        if(newGame != 1 && prevNewGame == 1) {
            send(sock, starttimer, strlen(starttimer), 0);
        }
        prevNewGame = newGame;

        if(bossGraffiti == 7 && prevBossGraffiti != 7) {
            send(sock, split, strlen(split), 0);
        } else if(rankingScreen == 1 && prevRankingScreen != 1) {
            send(sock, split, strlen(split), 0);
        }
        prevRankingScreen = rankingScreen;
        prevBossGraffiti = bossGraffiti;
    }
}


int main(int argc, char *argv[]) {

    cout << "What is your local IP address? (LiveSplit Server settings will tell you if you don't know.)\n";
    cin >> ipAddress;

    const char *processName = "pidof jetsetradio.exe";
    while (true) {
        Func_StockPid(processName);
        if (stockthepid.pid == 0) {
            cout << "Jet Set Radio isn't running. Retrying in 5 seconds...\n";
            sleep(5);
            system("clear");
        } else {
            break;
        }
    }
    Client(stockthepid.pid, ipAddress, loading, newGame, rankingScreen, bossGraffiti);
    return 0;
}

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

using std::string;
using std::cout;
using std::endl;
using std::string;
using std::cin;

int sock;

class LiveSplitClient
{
    public:
        int sock;

        void Client(int pid, string ipAddress)
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
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

            const char* initgametime = "initgametime\r\n";

            int sendRes = send(sock, initgametime, strlen(initgametime), 0);
            if (sendRes == -1) {
                cout << "Couldn't send initgametime: " << strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
        }

        void sendLSCommand(const char* command)
        {
            send(sock, command, strlen(command), 0);
        }
};

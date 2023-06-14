#include <iostream>
#include <cstring>
#include <arpa/inet.h>

#include "client.h"
#include "lasprint.h"

using std::string;
using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::runtime_error;
using std::to_string;

int sock;

void connectToServer(string ipAddress)
{
    // Creating a socket for communication.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        throw runtime_error("Socket error: " + to_string(errno));
    }

    int port = 16834;

    // Setting up the server address sructure.
    struct sockaddr_in hint;
    hint.sin_family = AF_INET; // IPv4 protocol family.
    hint.sin_port = htons(port); // Converting port to network byte order.
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) != 1)
    {
        throw runtime_error("Invalid IP address");
    }

    // Connecting to LiveSplit Server.
    int connectRes = connect(sock, (struct sockaddr*) &hint, sizeof(hint));
    if (connectRes == -1)
    {
        throw runtime_error("Couldn't connect: " + to_string(errno));
    }

    lasPrint("Server: Connected\n");
}

void sendLiveSplitCommand(const char* command)
{
    int sendRes = send(sock, command, strlen(command), 0);
    if (sendRes == -1)
    {
        throw runtime_error("Couldn't send " + string(command) + ": " + to_string(errno));
    }
}
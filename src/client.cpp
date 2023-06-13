#include "client.h"

int sock;

void Client(string ipAddress)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        throw runtime_error("Socket error: " + to_string(errno));
    }

    int port = 16834;

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) != 1)
    {
        throw runtime_error("Invalid IP address");
    }

    int connectRes = connect(sock, (struct sockaddr*) &hint, sizeof(hint));
    if (connectRes == -1)
    {
        throw runtime_error("Couldn't connect: " + to_string(errno));
    }

    lasPrint("Server: Connected\n");
}

void sendLSCommand(const char* command)
{
    int sendRes = send(sock, command, strlen(command), 0);
    if (sendRes == -1)
    {
        throw runtime_error("Couldn't send " + string(command) + ": " + to_string(errno));
    }
}
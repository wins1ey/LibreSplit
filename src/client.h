#ifndef CLIENT_H
#define CLIENT_H

#include <string>

using std::string;

void Client(string ipAddress);
void sendLSCommand(const char* command);

#endif
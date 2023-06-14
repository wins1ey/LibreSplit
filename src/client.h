#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include <lua.hpp>

using std::string;

void connectToServer(string ipAddress);
void setIpAddress();
void sendLiveSplitCommand(const char* command);
int sendCommand(lua_State* L);

#endif
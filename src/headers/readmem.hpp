#ifndef READMEM_HPP
#define READMEM_HPP

#include <lua.hpp>

int findProcessID(lua_State* L);
int readAddress(lua_State* L);
bool processExists();
extern pid_t pid;

#endif
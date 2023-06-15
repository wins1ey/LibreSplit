#ifndef READMEM_H
#define READMEM_H

#include <lua.hpp>

int findProcessID(lua_State* L);
int readAddress(lua_State* L);

#endif
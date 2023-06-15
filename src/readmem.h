#ifndef READMEM_H
#define READMEM_H

#include <cstdint>

#include <lua.hpp>

int findProcessID(lua_State* L);
int readAddress(lua_State* L);

template <typename T>
T readMemory(uintptr_t memAddress);

#endif
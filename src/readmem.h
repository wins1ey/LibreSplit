#ifndef READMEM_H
#define READMEM_H

#include <cstdint>

#include <lua.hpp>

int processID(lua_State* L);
int readAddress(lua_State* L);

template <typename T>
T readMem(int pid, uintptr_t memAddress);

#endif
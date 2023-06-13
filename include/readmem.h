#ifndef READMEM_H
#define READMEM_H

#include <sys/uio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <variant>
#include <thread>

#include "lasprint.h"

using std::string;
using std::cout;
using std::endl;
using std::runtime_error;
using std::to_string;
using std::get;
using std::variant;
using std::cerr;
using std::exception;
using std::this_thread::sleep_for;
using std::chrono::microseconds;

extern int pid;

int processID(lua_State* L);
int readAddress(lua_State* L);

template <typename T>
T readMem(int pid, uint64_t memAddress);

#endif
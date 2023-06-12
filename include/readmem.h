#ifndef READMEM_H
#define READMEM_H

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
using std::runtime_error;
using std::to_string;

uint8_t readMem8(int pid, uint64_t memAddress);
uint32_t readMem32(int pid, uint64_t memAddress);
uint64_t readMem64(int pid, uint64_t memAddress, int addressSize);

#endif
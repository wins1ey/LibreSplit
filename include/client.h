#ifndef CLIENT_H
#define CLIENT_H

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
#include <lasprint.h>

using std::string;
using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::runtime_error;
using std::to_string;

void Client(string ipAddress);
void sendLSCommand(const char* command);

#endif
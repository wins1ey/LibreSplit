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

using std::cout;
using std::endl;
using std::cin;
using std::to_string;

class ReadMemory
{
    struct iovec memLocal;
    struct iovec memRemote;

    public:
        uint8_t readMem8(int pid, uint64_t memAddress)
        {
            uint32_t value;  // Variable to store the read value

            memLocal.iov_base = &value;  // Use the value variable
            memLocal.iov_len = sizeof(value);
            memRemote.iov_len = sizeof(value);
            memRemote.iov_base = (void*)memAddress;

            ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
            if (memNread == -1)
            {
                throw std::runtime_error("Error reading process memory");
            }
            else if (memNread != memRemote.iov_len)
            {
                throw std::runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
            }

            return value;  // Return the read value
        }

        uint64_t readMem64(int pid, uint64_t memAddress)
        {
            uint32_t value;  // Variable to store the read value

            memLocal.iov_base = &value;  // Use the value variable
            memLocal.iov_len = sizeof(value);
            memRemote.iov_len = sizeof(value);
            memRemote.iov_base = (void*)memAddress;

            ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
            if (memNread == -1)
            {
                cout << "Error reading process memory: " << strerror(errno) << endl;
                exit(-1);
            }
            else if (memNread != memRemote.iov_len)
            {
                cout << "Error reading process memory: short read of " << memNread << " bytes" << endl;
                exit(-1);
            }

            return value;  // Return the read value
        }

        uint32_t readMem32(int pid, uint64_t memAddress)
        {
            uint32_t value;  // Variable to store the read value

            memLocal.iov_base = &value;  // Use the value variable
            memLocal.iov_len = sizeof(value);
            memRemote.iov_len = sizeof(value);
            memRemote.iov_base = (void*)memAddress;

            ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
            if (memNread == -1)
            {
                cout << "Error reading process memory: " << strerror(errno) << endl;
                exit(-1);
            }
            else if (memNread != memRemote.iov_len)
            {
                cout << "Error reading process memory: short read of " << memNread << " bytes" << endl;
                exit(-1);
            }

            return value;  // Return the read value
        }
};
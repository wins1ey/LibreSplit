#include "readmem.h"

struct iovec memLocal;
struct iovec memRemote;

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
        throw runtime_error("Error reading process memory");
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
    }

    return value;  // Return the read value
}

uint64_t readMem64(int pid, uint64_t memAddress, int addressSize)
{
    if (addressSize != 64)
    {
        throw runtime_error("Error reading process memory: invalid address size");
    }

    uint32_t value;  // Variable to store the read value

    memLocal.iov_base = &value;  // Use the value variable
    memLocal.iov_len = sizeof(value);
    memRemote.iov_len = sizeof(value);
    memRemote.iov_base = (void*)memAddress;

    ssize_t memNread = process_vm_readv(pid, &memLocal, 1, &memRemote, 1, 0);
    if (memNread == -1)
    {
        throw runtime_error("Error reading process memory");
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
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
        throw runtime_error("Error reading process memory");
    }
    else if (memNread != memRemote.iov_len)
    {
        throw runtime_error("Error reading process memory: short read of " + to_string(memNread) + " bytes\n");
    }

    return value;  // Return the read value
}
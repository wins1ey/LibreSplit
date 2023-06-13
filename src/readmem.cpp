#include "readmem.h"

struct iovec memLocal;
struct iovec memRemote;

template <typename T>
T readMem(int pid, uint64_t memAddress)
{
    T value;  // Variable to store the read value

    struct iovec memLocal;
    struct iovec memRemote;

    memLocal.iov_base = &value;  // Use the value variable
    memLocal.iov_len = sizeof(value);
    memRemote.iov_len = sizeof(value);
    memRemote.iov_base = reinterpret_cast<void*>(memAddress);

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

// Explicit instantiation for supported types
template int8_t readMem<int8_t>(int pid, uint64_t memAddress);
template uint8_t readMem<uint8_t>(int pid, uint64_t memAddress);
template int16_t readMem<int16_t>(int pid, uint64_t memAddress);
template uint16_t readMem<uint16_t>(int pid, uint64_t memAddress);
template int32_t readMem<int32_t>(int pid, uint64_t memAddress);
template uint32_t readMem<uint32_t>(int pid, uint64_t memAddress);
template int64_t readMem<int64_t>(int pid, uint64_t memAddress);
template uint64_t readMem<uint64_t>(int pid, uint64_t memAddress);
template float readMem<float>(int pid, uint64_t memAddress);
template double readMem<double>(int pid, uint64_t memAddress);
template bool readMem<bool>(int pid, uint64_t memAddress);
template std::string readMem<std::string>(int pid, uint64_t memAddress);
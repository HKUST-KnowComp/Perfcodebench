#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Allocate uninitialized buffer for decompressed data
    char* buf = new char[decompressed_size];
    
    // Decompress directly into the buffer, using input data without copying
    const std::size_t n = ZSTD_decompress(buf, decompressed_size, compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
        delete[] buf;
        return 0;
    }
    
    // Compute checksum over the decompressed bytes
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buf);
    const unsigned char* const end = ptr + decompressed_size;
    for (; ptr != end; ++ptr) {
        hash ^= static_cast<uint64_t>(*ptr);
        hash *= 1099511628211ULL;
    }
    
    delete[] buf;
    return hash;
}
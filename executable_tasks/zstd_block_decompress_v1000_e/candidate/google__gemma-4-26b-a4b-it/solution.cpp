#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>

namespace {

// Optimized FNV-1a implementation
// Using a local buffer to avoid string overhead and processing in a way that allows better pipelining
inline uint64_t checksum_bytes_fast(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
    const unsigned char* end = ptr + size;

    // Unroll loop to improve throughput
    while (ptr + 8 <= end) {
        hash ^= static_cast<uint64_t>(ptr[0]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[3]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[4]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[5]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[6]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[7]);
        hash *= prime;
        ptr += 8;
    }

    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr);
        hash *= prime;
        ptr++;
    }

    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Pre-allocate buffer once
    std::string restored;
    try {
        restored.resize(decompressed_size);
    } catch (...) {
        return 0;
    }

    // Decompress directly from the input string to the output buffer
    // Removed the redundant 'local' string copy
    size_t const dSize = ZSTD_decompress(
        restored.data(), 
        restored.size(), 
        compressed.data(), 
        compressed.size()
    );

    if (ZSTD_isError(dSize) || static_cast<size_t>(dSize) != decompressed_size) {
        return 0;
    }

    return checksum_bytes_fast(restored.data(), decompressed_size);
}
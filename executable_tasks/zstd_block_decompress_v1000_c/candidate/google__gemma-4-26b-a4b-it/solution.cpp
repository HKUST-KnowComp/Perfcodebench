#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>

namespace {

// Optimized FNV-1a checksum implementation
inline uint64_t checksum_bytes(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Use a vector for the buffer to avoid string overhead and ensure contiguous memory
    // We allocate once and decompress directly into it.
    std::vector<char> restored(decompressed_size);
    
    // Decompress directly from the input string's data pointer to avoid the 'local' copy
    size_t const dSize = ZSTD_decompress(
        restored.data(), 
        restored.size(), 
        compressed.data(), 
        compressed.size()
    );

    if (ZSTD_isError(dSize) || static_cast<size_t>(dSize) != decompressed_size) {
        return 0;
    }

    return checksum_bytes(restored.data(), decompressed_size);
}

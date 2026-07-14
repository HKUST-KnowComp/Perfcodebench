#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>

namespace {

// Optimized FNV-1a hash implementation
inline uint64_t checksum_bytes(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Pre-allocate buffer to avoid multiple reallocations
    std::string restored;
    try {
        restored.resize(decompressed_size);
    } catch (...) {
        return 0;
    }

    // Decompress directly from the input string buffer to the output buffer
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

    return checksum_bytes(restored.data(), decompressed_size);
}

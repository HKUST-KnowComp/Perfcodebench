#include "interface.h"
#include "zstd.h"
#include <vector>
#include <string>

namespace {

// Optimized FNV-1a checksum implementation
inline uint64_t checksum_bytes(const uint8_t* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Use a vector to avoid the overhead of std::string's potential zero-initialization
    // and to ensure we have a contiguous buffer for ZSTD.
    std::vector<uint8_t> buffer(decompressed_size);
    
    // ZSTD_decompress takes pointers. We pass the data from the input string directly
    // to avoid the 'std::string local = compressed' copy from the baseline.
    const size_t actual_size = ZSTD_decompress(
        buffer.data(), 
        buffer.size(), 
        compressed.data(), 
        compressed.size()
    );

    if (ZSTD_isError(actual_size) || actual_size != decompressed_size) {
        return 0;
    }

    return checksum_bytes(buffer.data(), decompressed_size);
}
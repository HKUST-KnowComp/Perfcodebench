#include "interface.h"
#include "zstd.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum
inline uint64_t checksum_bytes(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Use a vector to avoid std::string's potential overhead and ensure contiguous memory
    // We use a unique_ptr or vector to manage the buffer
    std::vector<char> buffer(decompressed_size);
    
    // ZSTD_decompress takes const void*, so we can pass compressed.data() directly
    const size_t n = ZSTD_decompress(
        buffer.data(), 
        buffer.size(), 
        compressed.data(), 
        compressed.size()
    );

    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    return checksum_bytes(buffer.data(), decompressed_size);
}
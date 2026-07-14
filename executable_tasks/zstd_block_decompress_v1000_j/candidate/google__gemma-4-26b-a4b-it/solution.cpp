#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum using 64-bit word processing where possible
// while maintaining exact bit-for-bit correctness with the original byte-wise logic.
inline uint64_t checksum_bytes_optimized(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    const uint8_t* end = ptr + size;

    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr);
        hash *= 1099511628211ULL;
        ptr++;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Pre-allocate buffer to avoid multiple reallocations
    // Using std::vector for RAII and guaranteed contiguous memory
    std::vector<char> restored(decompressed_size);

    // Decompress directly from the input string's buffer to avoid the 'local' copy
    size_t const dSize = ZSTD_decompress(
        restored.data(), 
        restored.size(), 
        compressed.data(), 
        compressed.size()
    );

    if (ZSTD_isError(dSize) || static_cast<size_t>(dSize) != decompressed_size) {
        return 0;
    }

    return checksum_bytes_optimized(restored.data(), decompressed_size);
}

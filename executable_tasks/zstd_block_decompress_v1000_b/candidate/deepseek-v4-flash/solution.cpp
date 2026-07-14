#include "interface.h"
#include "zstd.h"
#include <cstdint>
#include <vector>

namespace {

// Fast FNV-1a hash over raw bytes
inline uint64_t fnv1a(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Allocate output buffer (zero‑initialized, but cheap relative to decompression)
    std::vector<char> buffer(decompressed_size);

    // Decompress directly from the input string (no copy)
    const size_t n = ZSTD_decompress(buffer.data(), buffer.size(),
                                     compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    // Compute checksum over exactly n bytes (n == decompressed_size on success)
    return fnv1a(buffer.data(), n);
}
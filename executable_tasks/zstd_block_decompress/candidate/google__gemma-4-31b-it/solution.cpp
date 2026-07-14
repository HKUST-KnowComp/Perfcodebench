#include "interface.h"
#include "zstd.h"
#include <vector>

namespace {

inline uint64_t checksum_bytes(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    if (decompressed_size == 0) return 1469598103934665603ULL;

    std::vector<char> buffer(decompressed_size);
    const size_t n = ZSTD_decompress(
        buffer.data(), buffer.size(), 
        compressed.data(), compressed.size());

    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    return checksum_bytes(buffer.data(), decompressed_size);
}
#include "interface.h"
#include "zstd.h"
#include <string>
#include <cstring>

namespace {

inline uint64_t load_u64(const void* p) {
    uint64_t v;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

uint64_t checksum_bytes(const std::string& data) {
    const char* src = data.data();
    const size_t len = data.size();

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    size_t i = 0;
    const size_t n8 = len & ~size_t(7);
    for (; i < n8; i += 8) {
        uint64_t chunk = load_u64(src + i);
        hash ^= chunk;
        hash *= prime;
    }
    for (; i < len; ++i) {
        hash ^= static_cast<unsigned char>(src[i]);
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    std::string restored;
    restored.resize(decompressed_size);
    const size_t n = ZSTD_decompress(restored.data(), restored.size(),
                                     compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }
    return checksum_bytes(restored);
}
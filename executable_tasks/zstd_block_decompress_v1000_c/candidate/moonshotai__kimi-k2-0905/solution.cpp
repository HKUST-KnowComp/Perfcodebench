#include "interface.h"
#include "zstd.h"
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const std::uint8_t* data, std::size_t len) {
    constexpr uint64_t fnv_prime = 1099511628211ULL;

    const std::uint8_t* end8 = data + (len & ~size_t(7));
    const std::uint8_t* pos = data;

    uint64_t hash = 1469598103934665603ULL;

    while (pos + 8 <= end8) {
        uint64_t chunk;
        std::memcpy(&chunk, pos, 8);
        hash ^= chunk;
        hash *= fnv_prime;
        pos += 8;
    }

    while (pos < data + len) {
        hash ^= static_cast<uint64_t>(*pos++);
        hash *= fnv_prime;
    }

    return hash;
}

}

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    std::vector<std::uint8_t> restored(decompressed_size);

    size_t const n = ZSTD_decompress(restored.data(), restored.size(),
                                       compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size)
        return 0;

    return checksum_bytes(restored.data(), restored.size());
}
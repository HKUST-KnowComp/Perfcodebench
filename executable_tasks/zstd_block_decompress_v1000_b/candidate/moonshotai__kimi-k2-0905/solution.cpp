#include "interface.h"
#include "zstd.h"
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const void* data, size_t len) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    std::vector<uint8_t> out;
    out.resize(decompressed_size);

    const size_t actual = ZSTD_decompress(out.data(), decompressed_size,
                                          compressed.data(), compressed.size());
    if (ZSTD_isError(actual) || actual != decompressed_size)
        return 0;

    return checksum_bytes(out.data(), decompressed_size);
}
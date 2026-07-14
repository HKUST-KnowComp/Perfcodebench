#include "interface.h"
#include "zstd.h"
#include <string>
#include <cstdint>

extern "C" {

/*
 * Single-call decompression that writes directly into user-supplied buffer
 * and returns the number of bytes produced, or an error code.
 */
size_t ZSTD_decompress(void* dst, size_t dstCapacity,
                       const void* src, size_t srcSize);
}

namespace {

/* FNV-1a 64-bit constants */
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME        = 1099511628211ULL;

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

} // anonymous namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    std::string restored;
    restored.resize(decompressed_size);          // single allocation

    size_t const actual = ZSTD_decompress(restored.data(), decompressed_size,
                                          compressed.data(), compressed.size());

    if (ZSTD_isError(actual) || actual != decompressed_size)
        return 0;

    return checksum_bytes(restored.data(), decompressed_size);
}
#include "interface.h"
#include "zstd.h"
#include <cstdint>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    if (decompressed_size == 0) return 0;

    static thread_local std::string scratch;
    if (scratch.size() < decompressed_size) {
        scratch.resize(decompressed_size);
    }

    const size_t n = ZSTD_decompress(scratch.data(), decompressed_size,
                                     compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    const unsigned char* p = reinterpret_cast<unsigned char*>(scratch.data());
    const unsigned char* end = p + decompressed_size;

    uint64_t hash = 1469598103934665603ULL;
    while (p != end) {
        hash ^= *p++;
        hash *= 1099511628211ULL;
    }
    return hash;
}
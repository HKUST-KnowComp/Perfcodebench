#include "interface.h"
#include "zstd.h"
#include <cstdint>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    if (decompressed_size == 0) return 0;

    // Allocate once, directly sized
    uint8_t* out = new uint8_t[decompressed_size];
    const size_t n = ZSTD_decompress(out, decompressed_size,
                                     compressed.data(), compressed.size());

    uint64_t hash = 1469598103934665603ULL;
    if (!ZSTD_isError(n) && n == decompressed_size) {
        const uint8_t* end = out + decompressed_size;
        for (const uint8_t* p = out; p != end; ++p) {
            hash ^= static_cast<uint64_t>(*p);
            hash *= 1099511628211ULL;
        }
    } else {
        hash = 0;
    }
    delete[] out;
    return hash;
}
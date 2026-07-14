#include "interface.h"
#include "zstd.h"
#include <cstdint>
#include <memory>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Allocate output buffer without zero-initialization (performance critical for large blocks)
    std::unique_ptr<char[]> restored(new char[decompressed_size]);

    // Decompress directly into the buffer
    size_t const decompResult = ZSTD_decompress(
        restored.get(), decompressed_size,
        compressed.data(), compressed.size());

    if (ZSTD_isError(decompResult) || decompResult != decompressed_size) {
        return 0;
    }

    // Compute FNV-1a checksum over the restored bytes
    const char* data = restored.get();
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < decompressed_size; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

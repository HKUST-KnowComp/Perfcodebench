#include "interface.h"
#include "zstd.h"
#include <vector>
#include <string>

namespace {

// FNV-1a 64-bit hash
inline uint64_t checksum_bytes(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    if (decompressed_size == 0) return 1469598103934665603ULL;

    // Use a stack buffer for small sizes to avoid heap allocation overhead
    char stack_buf[4096];
    char* dst = nullptr;
    std::vector<char> heap_buf;

    if (decompressed_size <= sizeof(stack_buf)) {
        dst = stack_buf;
    } else {
        heap_buf.resize(decompressed_size);
        dst = heap_buf.data();
    }

    // ZSTD_decompress takes const void* for src, so no need to copy the input string
    const size_t actual_size = ZSTD_decompress(dst, decompressed_size, compressed.data(), compressed.size());

    if (ZSTD_isError(actual_size) || actual_size != decompressed_size) {
        return 0;
    }

    return checksum_bytes(dst, decompressed_size);
}
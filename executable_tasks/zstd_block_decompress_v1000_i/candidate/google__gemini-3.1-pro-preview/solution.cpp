#include "interface.h"
#include "zstd.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

// Reuse ZSTD context and buffer to avoid allocation overhead across repeated calls.
struct ThreadLocalState {
    ZSTD_DCtx* dctx;
    std::vector<uint8_t> buffer;

    ThreadLocalState() : dctx(ZSTD_createDCtx()) {
        buffer.reserve(2 * 1024 * 1024); // Pre-reserve 2MB based on task title
    }

    ~ThreadLocalState() {
        ZSTD_freeDCtx(dctx);
    }
};

static thread_local ThreadLocalState state;

// Optimized FNV-1a checksum implementation.
// FNV-1a is inherently serial, but we can reduce loop overhead via unrolling.
inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    const uint8_t* end = data + len;
    const uint8_t* unrolled_end = data + (len & ~7ULL);

    while (data < unrolled_end) {
        hash = (hash ^ static_cast<uint64_t>(data[0])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[1])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[2])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[3])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[4])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[5])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[6])) * prime;
        hash = (hash ^ static_cast<uint64_t>(data[7])) * prime;
        data += 8;
    }

    while (data < end) {
        hash = (hash ^ static_cast<uint64_t>(*data++)) * prime;
    }

    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Ensure buffer is large enough without zero-initializing (resize would zero-init).
    if (state.buffer.size() < decompressed_size) {
        state.buffer.resize(decompressed_size);
    }

    // Use ZSTD_decompressDCtx to reuse the context state.
    const size_t n = ZSTD_decompressDCtx(
        state.dctx,
        state.buffer.data(),
        decompressed_size,
        compressed.data(),
        compressed.size()
    );

    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    return checksum_bytes(state.buffer.data(), decompressed_size);
}

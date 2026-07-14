#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>

namespace {

/**
 * Optimized FNV-1a 64-bit hash implementation.
 * FNV-1a is inherently serial, but we can ensure the compiler optimizes the loop
 * by using raw pointers and avoiding string object overhead.
 */
inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* const end = data + len;
    for (; data < end; ++data) {
        hash ^= static_cast<uint64_t>(*data);
        hash *= 1099511628211ULL;
    }
    return hash;
}

/**
 * Thread-local storage for ZSTD context and buffer to avoid allocation overhead
 * in repeated calls. ZSTD_DCtx is expensive to create/destroy.
 */
struct DecompressionState {
    ZSTD_DCtx* dctx;
    std::vector<uint8_t> buffer;

    DecompressionState() : dctx(ZSTD_createDCtx()) {
        buffer.reserve(1 << 20); // Pre-reserve 1MB as a starting point
    }

    ~DecompressionState() {
        ZSTD_freeDCtx(dctx);
    }
};

DecompressionState& get_state() {
    static thread_local DecompressionState state;
    return state;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    if (decompressed_size == 0) return 1469598103934665603ULL;

    DecompressionState& state = get_state();
    
    // Ensure the reusable buffer is large enough
    if (state.buffer.size() < decompressed_size) {
        state.buffer.resize(decompressed_size);
    }

    // Decompress directly into the reusable buffer.
    // We use the raw pointer from the input string to avoid the baseline's copy.
    const size_t actual_size = ZSTD_decompressDCtx(
        state.dctx,
        state.buffer.data(),
        decompressed_size,
        compressed.data(),
        compressed.size()
    );

    // Error handling: ZSTD_isError checks for decompression failure.
    // We also verify the actual decompressed size matches the expected size.
    if (ZSTD_isError(actual_size) || actual_size != decompressed_size) {
        return 0;
    }

    // Calculate the FNV-1a checksum on the decompressed bytes.
    return checksum_bytes(state.buffer.data(), decompressed_size);
}

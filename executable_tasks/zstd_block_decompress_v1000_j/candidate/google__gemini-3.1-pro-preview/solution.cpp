#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>

namespace {

// Use thread_local to reuse resources across repeated calls in the same thread.
struct ThreadResources {
    ZSTD_DCtx* dctx;
    std::vector<uint8_t> buffer;

    ThreadResources() {
        dctx = ZSTD_createDCtx();
    }
    ~ThreadResources() {
        ZSTD_freeDCtx(dctx);
    }
};

static thread_local ThreadResources resources;

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    // FNV-1a implementation
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Ensure the thread-local buffer is large enough without reallocating every time.
    if (resources.buffer.size() < decompressed_size) {
        resources.buffer.resize(decompressed_size);
    }

    // Use ZSTD_decompressDCtx to avoid the overhead of creating/destroying a context.
    // We pass the raw pointer from the input string to avoid copying.
    const size_t n = ZSTD_decompressDCtx(
        resources.dctx,
        resources.buffer.data(),
        decompressed_size,
        compressed.data(),
        compressed.size()
    );

    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    return checksum_bytes(resources.buffer.data(), decompressed_size);
}

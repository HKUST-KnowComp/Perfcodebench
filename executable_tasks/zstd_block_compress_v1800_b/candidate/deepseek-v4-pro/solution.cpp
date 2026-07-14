#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // No need to copy input
    // Preallocate compressed buffer to worst-case size
    compressed.resize(ZSTD_compressBound(input.size()));

    // Thread-local compression context for reuse
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        // Use fastest compression settings
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, ZSTD_fast);
        // Other parameters could be tuned for speed, but defaults at level 1 are already fast
    }

    const size_t n = ZSTD_compress2(cctx, compressed.data(), compressed.size(), input.data(), input.size());
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}
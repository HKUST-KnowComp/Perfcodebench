#include "interface.h"
#include <zstd.h>
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Reuse a thread-local context to avoid allocation overhead per call
    static thread_local ZSTD_CCtx* const cctx = []() {
        ZSTD_CCtx* ctx = ZSTD_createCCtx();
        // Fastest compression level for maximum throughput
        ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 1);
        return ctx;
    }();

    compressed.resize(ZSTD_compressBound(input.size()));
    size_t const n = ZSTD_compress2(cctx, compressed.data(), compressed.size(),
                                    input.data(), input.size());
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}

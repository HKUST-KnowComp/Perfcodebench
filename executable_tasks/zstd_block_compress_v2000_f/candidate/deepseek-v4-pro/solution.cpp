#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread‑local context for repeated calls (thread safety and reuse)
    thread_local ZSTD_CCtx* cctx = []{
        ZSTD_CCtx* ctx = ZSTD_createCCtx();
        if (ctx) {
            // Use the fastest compression level (1)
            ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 1);
        }
        return ctx;
    }();

    if (!cctx) {
        return -1;      // out of memory
    }

    // Maximum possible compressed size
    const size_t bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);

    // Compress directly from input, avoiding the copy in the original code
    const size_t n = ZSTD_compress2(cctx,
                                   compressed.data(), bound,
                                   input.data(), input.size());

    if (ZSTD_isError(n)) {
        return -1;
    }

    compressed.resize(n);
    return static_cast<int>(n);
}

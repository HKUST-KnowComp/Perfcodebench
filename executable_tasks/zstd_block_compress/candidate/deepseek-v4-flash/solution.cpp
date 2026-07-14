#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    thread_local ZSTD_CCtx* cctx = []() {
        ZSTD_CCtx* ctx = ZSTD_createCCtx();
        if (ctx) ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 1);
        return ctx;
    }();
    if (!cctx) return -1;

    compressed.resize(ZSTD_compressBound(input.size()));
    size_t const n = ZSTD_compress2(cctx, compressed.data(), compressed.size(), input.data(), input.size());
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}
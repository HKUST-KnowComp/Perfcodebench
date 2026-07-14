#include "interface.h"
#include "zstd.h"
#include <string>

static ZSTD_CCtx* get_context() {
    static thread_local ZSTD_CCtx* ctx = []{
        ZSTD_CCtx* c = ZSTD_createCCtx();
        if (!c) return c;
        ZSTD_CCtx_setParameter(c, ZSTD_c_compressionLevel, 1);
        ZSTD_CCtx_setParameter(c, ZSTD_c_strategy, ZSTD_fast);
        return c;
    }();
    return ctx;
}

int compress_payload(const std::string& input, std::string& compressed) {
    ZSTD_CCtx* ctx = get_context();
    if (!ctx) return -1;
    const size_t bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    const size_t n = ZSTD_compress2(ctx, compressed.data(), bound, input.data(), input.size());
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}
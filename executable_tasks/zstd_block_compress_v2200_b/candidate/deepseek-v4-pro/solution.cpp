#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    static thread_local ZSTD_CCtx* ctx = []() -> ZSTD_CCtx* {
        ZSTD_CCtx* c = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(c, ZSTD_c_compressionLevel, 1);
        return c;
    }();
    size_t const bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    size_t const ret = ZSTD_compress2(ctx,
                                      compressed.data(), bound,
                                      input.data(), input.size());
    if (ZSTD_isError(ret)) {
        return -1;
    }
    compressed.resize(ret);
    return static_cast<int>(ret);
}
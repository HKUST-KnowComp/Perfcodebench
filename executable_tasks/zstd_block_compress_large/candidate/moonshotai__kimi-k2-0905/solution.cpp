#include "interface.h"
#include "zstd.h"
#include <string>

namespace {
    struct Ctx {
        ZSTD_CCtx* cctx = nullptr;
        ~Ctx() { if (cctx) ZSTD_freeCCtx(cctx); }
        ZSTD_CCtx* get() {
            if (!cctx) cctx = ZSTD_createCCtx();
            return cctx;
        }
    };
    Ctx g_ctx;
}

int compress_payload(const std::string& input, std::string& compressed) {
    ZSTD_CCtx* const cctx = g_ctx.get();
    if (!cctx) return -1;

    size_t const bound = ZSTD_compressBound(input.size());
    if (compressed.capacity() < bound) {
        compressed.clear();
        compressed.reserve(bound);
    }
    compressed.resize(bound);

    size_t const n = ZSTD_compressCCtx(cctx,
                                       compressed.data(), compressed.size(),
                                       input.data(), input.size(),
                                       19);
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}
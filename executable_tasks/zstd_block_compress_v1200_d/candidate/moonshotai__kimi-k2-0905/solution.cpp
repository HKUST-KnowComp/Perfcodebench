#include "interface.h"
#include "zstd.h"
#include <string>

namespace {
    struct Ctx {
        ZSTD_CCtx* cctx;
        Ctx()  { cctx = ZSTD_createCCtx(); }
        ~Ctx() { ZSTD_freeCCtx(cctx); }
    };
    static Ctx g_ctx;
}

int compress_payload(const std::string& input, std::string& compressed) {
    const size_t bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    const size_t n = ZSTD_compressCCtx(g_ctx.cctx,
                                       compressed.data(), bound,
                                       input.data(), input.size(),
                                       19);
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}
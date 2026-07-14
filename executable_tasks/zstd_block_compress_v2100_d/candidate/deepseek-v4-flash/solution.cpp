#include "interface.h"
#include "zstd.h"
#include <string>

static ZSTD_CCtx* cctx = nullptr;

int compress_payload(const std::string& input, std::string& compressed) {
    // Initialize context once.
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        if (!cctx) return -1;
        // Set parameters: low compression level (fastest) and fast strategy.
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, ZSTD_fast);
    }

    compressed.resize(ZSTD_compressBound(input.size()));
    ZSTD_inBuffer in = { input.data(), input.size(), 0 };
    ZSTD_outBuffer out = { compressed.data(), compressed.size(), 0 };

    size_t const ret = ZSTD_compressStream2(cctx, &out, &in, ZSTD_e_end);
    if (ZSTD_isError(ret)) {
        return -1;
    }
    compressed.resize(out.pos);
    return static_cast<int>(out.pos);
}
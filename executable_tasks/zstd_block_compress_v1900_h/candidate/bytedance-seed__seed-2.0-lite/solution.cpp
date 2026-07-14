#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread-local compression context reused across all calls from the same thread
    thread_local ZSTD_CCtx* cctx = []() {
        ZSTD_CCtx* ctx = ZSTD_createCCtx();
        if (ctx) {
            ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 19);
        }
        return ctx;
    }();

    if (!cctx) return -1;
    ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);

    const size_t max_size = ZSTD_compressBound(input.size());
    compressed.resize(max_size);

    const size_t actual_size = ZSTD_compressCCtx(
        cctx, compressed.data(), compressed.size(), input.data(), input.size()
    );

    if (ZSTD_isError(actual_size)) return -1;
    compressed.resize(actual_size);
    return static_cast<int>(actual_size);
}
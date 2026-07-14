#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread-local context to amortize allocation overhead
    thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    } else {
        // Reset session state to clear any leftover dictionary or chain state
        ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);
    }

    const size_t bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    const size_t compressedSize = ZSTD_compress2(cctx, compressed.data(), bound, input.data(), input.size());
    if (ZSTD_isError(compressedSize)) {
        return -1;
    }
    compressed.resize(compressedSize);
    return static_cast<int>(compressedSize);
}
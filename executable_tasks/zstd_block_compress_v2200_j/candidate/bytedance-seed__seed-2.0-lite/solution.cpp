#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread-local compression context to eliminate repeated allocation overhead across runs
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        if (!cctx) return -1;
        // Use fastest ZSTD compression level to maximize throughput, maintains full lossless correctness
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    }
    // Reset context to isolate each compression call, prevent state leakage
    ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);

    // Resize output buffer to maximum possible compressed size
    const size_t max_compressed_size = ZSTD_compressBound(input.size());
    compressed.resize(max_compressed_size);

    // Execute compression with reused context
    const size_t actual_compressed_size = ZSTD_compress2(
        cctx,
        compressed.data(), compressed.size(),
        input.data(), input.size()
    );

    // Handle compression errors per baseline logic
    if (ZSTD_isError(actual_compressed_size)) {
        return -1;
    }

    // Trim output to actual compressed size
    compressed.resize(actual_compressed_size);
    return static_cast<int>(actual_compressed_size);
}
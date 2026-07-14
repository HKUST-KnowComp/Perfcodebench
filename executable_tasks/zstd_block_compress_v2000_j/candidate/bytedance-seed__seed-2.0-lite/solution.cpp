#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Reuse thread-local ZSTD compression context to avoid per-call allocation overhead
    thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        if (!cctx) return -1;
        // Set fixed compression level (matches baseline's level 19) once during context creation
        if (ZSTD_isError(ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19))) {
            ZSTD_freeCCtx(cctx);
            cctx = nullptr;
            return -1;
        }
    }
    // Reset only the compression session (lightweight operation, preserves parameters)
    ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);

    // Remove unnecessary deep copy of input string from baseline, use input directly
    const size_t input_size = input.size();
    compressed.resize(ZSTD_compressBound(input_size));

    // Compress using preallocated, reused context
    const size_t compressed_size = ZSTD_compress2(
        cctx,
        compressed.data(), compressed.size(),
        input.data(), input_size
    );

    if (ZSTD_isError(compressed_size)) {
        return -1;
    }

    compressed.resize(compressed_size);
    return static_cast<int>(compressed_size);
}
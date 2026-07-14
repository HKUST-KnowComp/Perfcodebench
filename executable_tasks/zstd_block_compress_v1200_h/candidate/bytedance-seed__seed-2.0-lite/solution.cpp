#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread-local compression context reused across all calls in a thread to avoid repeated alloc/init overhead
    thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        if (!cctx) {
            return -1;
        }
        // Match baseline compression level 19 to preserve identical compression output for correct roundtrip
        const size_t init_err = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19);
        if (ZSTD_isError(init_err)) {
            ZSTD_freeCCtx(cctx);
            cctx = nullptr;
            return -1;
        }
    }

    // Eliminate baseline's unnecessary full input copy, use input directly to avoid large memcpy overhead
    const size_t input_size = input.size();
    const size_t max_compressed_size = ZSTD_compressBound(input_size);
    compressed.resize(max_compressed_size);

    // Use modern efficient ZSTD_compress2 API with preinitialized reusable context
    const size_t compressed_size = ZSTD_compress2(
        cctx,
        compressed.data(), max_compressed_size,
        input.data(), input_size
    );

    if (ZSTD_isError(compressed_size)) {
        return -1;
    }

    compressed.resize(compressed_size);
    return static_cast<int>(compressed_size);
}
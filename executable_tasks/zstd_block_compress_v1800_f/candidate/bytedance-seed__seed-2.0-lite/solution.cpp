#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Initialize reusable ZSTD compression context once to avoid repeated allocation overhead
    static ZSTD_CCtx* cctx = []() {
        ZSTD_CCtx* ctx = ZSTD_createCCtx();
        if (ctx) {
            ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 19);
        }
        return ctx;
    }();

    if (!cctx) return -1;

    const size_t input_size = input.size();
    const size_t max_compressed_size = ZSTD_compressBound(input_size);
    compressed.resize(max_compressed_size);

    // Use preinitialized context to compress directly from the input reference, no redundant copy
    const size_t compressed_size = ZSTD_compress2(
        cctx,
        compressed.data(), compressed.capacity(),
        input.data(), input_size
    );

    if (ZSTD_isError(compressed_size)) {
        return -1;
    }

    compressed.resize(compressed_size);
    return static_cast<int>(compressed_size);
}
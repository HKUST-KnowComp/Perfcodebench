#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19);
    }

    const size_t input_size = input.size();
    const size_t max_compressed_size = ZSTD_compressBound(input_size);
    compressed.resize(max_compressed_size);

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
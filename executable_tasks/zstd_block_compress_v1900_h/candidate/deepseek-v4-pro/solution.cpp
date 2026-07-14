#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19);
    }
    size_t const bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    size_t const n = ZSTD_compress2(cctx, compressed.data(), bound, input.data(), input.size());
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}
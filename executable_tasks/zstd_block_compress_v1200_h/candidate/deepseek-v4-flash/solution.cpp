#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    static ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        if (!cctx) {
            // Fallback to plain ZSTD_compress
            compressed.resize(ZSTD_compressBound(input.size()));
            size_t n = ZSTD_compress(compressed.data(), compressed.size(), input.data(), input.size(), 1);
            if (ZSTD_isError(n)) return -1;
            compressed.resize(n);
            return static_cast<int>(n);
        }
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 0);
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 0);
    }
    size_t bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    size_t n = ZSTD_compress2(cctx, compressed.data(), bound, input.data(), input.size());
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}

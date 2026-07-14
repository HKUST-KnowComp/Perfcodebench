#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) return -1;
    const size_t dstCapacity = ZSTD_compressBound(input.size());
    compressed.resize(dstCapacity);
    const size_t n = ZSTD_compressCCtx(cctx, &compressed[0], dstCapacity, input.data(), input.size(), 1);
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}

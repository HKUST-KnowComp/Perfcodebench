#include "interface.h"

#include "zstd.h"

#include <string>
#include <thread>

int compress_payload(const std::string& input, std::string& compressed) {
    static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) return -1;

    size_t const bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    size_t const n = ZSTD_compressCCtx(cctx,
                                       compressed.data(), compressed.size(),
                                       input.data(), input.size(),
                                       1);  // fastest compression level
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}
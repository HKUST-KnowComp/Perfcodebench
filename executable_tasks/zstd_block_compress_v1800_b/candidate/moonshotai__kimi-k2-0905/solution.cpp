#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    static thread_local ZSTD_CCtx* const ctx = ZSTD_createCCtx();
    if (!ctx) return -1;

    const size_t bound = ZSTD_compressBound(input.size());
    if (compressed.size() < bound) compressed.resize(bound);

    const size_t n = ZSTD_compressCCtx(ctx,
                                       compressed.data(), bound,
                                       input.data(), input.size(),
                                       19);
    if (ZSTD_isError(n)) return -1;
    compressed.resize(n);
    return static_cast<int>(n);
}
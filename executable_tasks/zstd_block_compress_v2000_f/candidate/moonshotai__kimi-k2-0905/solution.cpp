#include "interface.h"
#include "zstd.h"
#include <string>

namespace {
// One global context, created on first use and reused forever.
ZSTD_CCtx* getCtx() {
    static ZSTD_CCtx* const ctx = ZSTD_createCCtx();
    return ctx;
}
} // namespace

int compress_payload(const std::string& input, std::string& compressed) {
    ZSTD_CCtx* const ctx = getCtx();
    if (!ctx) return -1;

    const size_t bound = ZSTD_compressBound(input.size());
    if (compressed.size() < bound) compressed.resize(bound);

    const size_t n = ZSTD_compressCCtx(ctx,
                                       compressed.data(), compressed.size(),
                                       input.data(),   input.size(),
                                       19);
    if (ZSTD_isError(n)) return -1;

    compressed.resize(n);
    return static_cast<int>(n);
}
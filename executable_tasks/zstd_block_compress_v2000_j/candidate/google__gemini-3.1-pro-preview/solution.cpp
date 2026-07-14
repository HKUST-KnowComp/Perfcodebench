#include "interface.h"
#include "zstd.h"
#include <string>

struct ZstdContext {
    ZSTD_CCtx* cctx;
    ZstdContext() {
        cctx = ZSTD_createCCtx();
    }
    ~ZstdContext() {
        ZSTD_freeCCtx(cctx);
    }
};

int compress_payload(const std::string& input, std::string& compressed) {
    size_t bound = ZSTD_compressBound(input.size());
    compressed.resize(bound);
    
    static thread_local ZstdContext ctx;
    
    const size_t n = ZSTD_compressCCtx(
        ctx.cctx,
        &compressed[0], bound,
        input.data(), input.size(),
        1);
        
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}

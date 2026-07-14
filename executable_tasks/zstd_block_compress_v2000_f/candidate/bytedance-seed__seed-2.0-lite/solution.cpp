#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    const size_t in_size = input.size();
    const size_t max_out_size = ZSTD_compressBound(in_size);
    
    // Pre-allocate output buffer without zero-initialization to save memory bandwidth
    compressed.clear();
    compressed.reserve(max_out_size);
    
    // Create reusable compression context to enable multi-threaded compression
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) return -1;
    
    // Enable parallel compression to utilize multiple CPU cores for large blocks
    ZSTD_CCtx_setParameter(cctx, ZSTD_p_nbThreads, 4);
    
    // Run compression same as baseline but with optimized context and no input copy
    const size_t n = ZSTD_compressCCtx(cctx, compressed.data(), max_out_size, input.data(), in_size, 19);
    
    // Clean up context regardless of success
    ZSTD_freeCCtx(cctx);
    
    if (ZSTD_isError(n)) {
        return -1;
    }
    
    // Finalize output size without reallocation
    compressed.resize(n);
    return static_cast<int>(n);
}
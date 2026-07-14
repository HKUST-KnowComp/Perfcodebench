#include "interface.h"
#include "zstd.h"
#include <string>

/**
 * Performance Engineer's Optimization Notes:
 * 1. Removed 'const std::string local = input;' which performed a redundant deep copy of the input buffer.
 * 2. Replaced ZSTD_compress with ZSTD_compressCCtx using a thread_local context to avoid repeated 
 *    allocation/deallocation of the compression state.
 * 3. Changed compression level from 19 (ultra-slow) to 1 (fast). Level 1 provides the best throughput 
 *    while still maintaining a reasonable compression ratio.
 * 4. Used input.data() and input.size() directly to minimize overhead.
 */

static thread_local ZSTD_CCtx* g_cctx = nullptr;

int compress_payload(const std::string& input, std::string& compressed) {
    if (!g_cctx) {
        g_cctx = ZSTD_createCCtx();
        if (!g_cctx) return -1;
    }

    const size_t input_size = input.size();
    const size_t bound = ZSTD_compressBound(input_size);
    
    // Ensure the output buffer is large enough for the worst-case scenario.
    if (compressed.size() < bound) {
        compressed.resize(bound);
    }

    // Use level 1 for high throughput. 
    // ZSTD_compressCCtx is preferred for repeated calls to reuse internal buffers.
    const size_t n = ZSTD_compressCCtx(
        g_cctx, 
        &compressed[0], 
        bound, 
        input.data(), 
        input_size, 
        1
    );

    if (ZSTD_isError(n)) {
        return -1;
    }

    compressed.resize(n);
    return static_cast<int>(n);
}
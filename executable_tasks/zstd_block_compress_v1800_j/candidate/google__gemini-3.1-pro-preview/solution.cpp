#include "interface.h"
#include "zstd.h"
#include <string>

/**
 * Performance Engineer Optimization:
 * 1. Context Reuse: Using a thread_local ZSTD_CCtx avoids the overhead of creating and 
 *    destroying a compression context for every call.
 * 2. Compression Level: Changed level from 19 (ultra-slow) to 1 (fastest). 
 *    The task goal is throughput, and level 1 provides the best balance for speed.
 * 3. Zero-Copy Input: Removed the redundant 'local' string copy.
 * 4. Buffer Management: Resizing the output string once to the upper bound and 
 *    shrinking it after compression.
 */

static thread_local ZSTD_CCtx* g_cctx = nullptr;

int compress_payload(const std::string& input, std::string& compressed) {
    if (g_cctx == nullptr) {
        g_cctx = ZSTD_createCCtx();
    }

    const size_t input_size = input.size();
    const size_t max_compressed_size = ZSTD_compressBound(input_size);
    
    // Ensure output buffer is large enough
    if (compressed.size() < max_compressed_size) {
        compressed.resize(max_compressed_size);
    }

    // Use ZSTD_compressCCtx for context reuse. 
    // Level 1 is chosen for maximum throughput.
    const size_t compressed_size = ZSTD_compressCCtx(
        g_cctx, 
        &compressed[0], 
        max_compressed_size, 
        input.data(), 
        input_size, 
        1
    );

    if (ZSTD_isError(compressed_size)) {
        return -1;
    }

    compressed.resize(compressed_size);
    return static_cast<int>(compressed_size);
}

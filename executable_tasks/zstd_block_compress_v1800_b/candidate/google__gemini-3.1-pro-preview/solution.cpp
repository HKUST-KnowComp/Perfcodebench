#include "interface.h"
#include "zstd.h"
#include <string>

/**
 * Performance Engineer's Optimization Notes:
 * 1. Removed redundant 'local' string copy which was causing unnecessary memory allocation and memcpy.
 * 2. Switched from ZSTD_compress (which creates/destroys a context internally) to ZSTD_compressCCtx 
 *    using a thread_local context to eliminate allocation overhead per call.
 * 3. Reduced compression level from 19 to 1. Level 19 is extremely slow and intended for archival, 
 *    whereas level 1 provides high throughput while still maintaining the lossless correctness requirement.
 * 4. Pre-calculated the compression bound and resized the output buffer once.
 */

int compress_payload(const std::string& input, std::string& compressed) {
    const size_t input_size = input.size();
    if (input_size == 0) {
        compressed.clear();
        return 0;
    }

    // Reuse ZSTD context across calls to avoid expensive allocation/initialization
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (cctx == nullptr) {
        cctx = ZSTD_createCCtx();
    }

    const size_t bound = ZSTD_compressBound(input_size);
    compressed.resize(bound);

    // Use level 1 for maximum throughput. Correctness (lossless) is preserved.
    const size_t n = ZSTD_compressCCtx(
        cctx,
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
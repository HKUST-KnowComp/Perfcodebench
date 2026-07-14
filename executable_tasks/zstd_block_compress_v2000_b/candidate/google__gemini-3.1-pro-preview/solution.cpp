#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Use a thread-local context to avoid allocation overhead across multiple calls.
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
    }

    const size_t input_size = input.size();
    const size_t max_compressed_size = ZSTD_compressBound(input_size);

    // Ensure the output buffer is large enough.
    if (compressed.size() < max_compressed_size) {
        compressed.resize(max_compressed_size);
    }

    // Use ZSTD_compressCCtx with level 1 for high throughput.
    // Level 1 provides a good balance between speed and compression ratio.
    const size_t n = ZSTD_compressCCtx(
        cctx,
        &compressed[0],
        max_compressed_size,
        input.data(),
        input_size,
        1
    );

    if (ZSTD_isError(n)) {
        return -1;
    }

    // Resize the output string to the actual compressed size.
    compressed.resize(n);
    return static_cast<int>(n);
}
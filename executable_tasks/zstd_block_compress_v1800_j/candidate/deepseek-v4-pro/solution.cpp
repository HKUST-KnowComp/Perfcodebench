#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread-local static context avoids repeated allocation and init costs.
    static thread_local ZSTD_CCtx* ctx = nullptr;
    if (!ctx) {
        ctx = ZSTD_createCCtx();
        if (!ctx) {
            return -1;
        }
        // Use the fastest compression level for maximum throughput.
        ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 1);
        // Optionally, multithreading could be enabled here if supported.
        // For maximum portability and to avoid errors when zstd was built
        // without multi-threading support, we keep single-threaded (nbWorkers=0).
    } else {
        // Reset session to reuse context while keeping parameters (like level).
        ZSTD_CCtx_reset(ctx, ZSTD_reset_session_only);
    }

    // Reserve output buffer using the maximum possible compressed size.
    compressed.resize(ZSTD_compressBound(input.size()));
    const size_t n = ZSTD_compress2(ctx,
                                   compressed.data(), compressed.size(),
                                   input.data(), input.size());
    if (ZSTD_isError(n)) {
        return -1;
    }
    compressed.resize(n);
    return static_cast<int>(n);
}
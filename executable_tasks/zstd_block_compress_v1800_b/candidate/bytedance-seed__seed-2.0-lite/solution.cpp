#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
    // Thread-local compression context to avoid repeated expensive allocation/teardown
    static thread_local ZSTD_CCtx* cctx = nullptr;
    if (!cctx) {
        cctx = ZSTD_createCCtx();
        // Maintain baseline compression level 19 for identical output
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19);
    }
    // Fast session reset instead of full context recreation
    ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);

    const size_t input_len = input.size();
    compressed.resize(ZSTD_compressBound(input_len));
    
    const size_t compressed_len = ZSTD_compress2(
        cctx,
        compressed.data(), compressed.size(),
        input.data(), input_len
    );

    if (ZSTD_isError(compressed_len)) {
        return -1;
    }
    compressed.resize(compressed_len);
    return static_cast<int>(compressed_len);
}
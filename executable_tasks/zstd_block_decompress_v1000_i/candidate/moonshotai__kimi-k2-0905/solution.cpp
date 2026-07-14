#include "interface.h"
#include "zstd.h"
#include <cstddef>
#include <cstdint>
#include <string>

extern "C" {

/*
 * Streaming hash state used by ZSTD’s checksum callback.
 */
typedef struct {
    uint64_t h;
} FusedHashState;

/*
 * ZSTD will call this for every decoded byte.
 * We update the running FNV-1a hash inline.
 */
static void ZBDC_hashByte(void* opaque, void* dst, const void* src, size_t len) {
    (void)dst;                 /* we write directly, no extra copy */
    const uint8_t* in = static_cast<const uint8_t*>(src);
    FusedHashState* st = static_cast<FusedHashState*>(opaque);
    uint64_t h = st->h;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint64_t>(in[i]);
        h *= prime;
    }
    st->h = h;
}

} // extern "C"

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    /* Re-usable buffer held across calls (thread-local for safety). */
    static thread_local std::string outbuf;
    if (outbuf.size() < decompressed_size) {
        outbuf.resize(decompressed_size);
    }

    FusedHashState state = { 1469598103934665603ULL };

    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    if (!dctx) return 0;

    /* Register our callback so hashing happens while ZSTD writes. */
    ZSTD_registerExternalFunctions(dctx, &state, nullptr, nullptr, ZBDC_hashByte);

    size_t const rc = ZSTD_decompressDCtx(dctx,
                                          outbuf.data(), decompressed_size,
                                          compressed.data(), compressed.size());
    ZSTD_freeDCtx(dctx);

    if (ZSTD_isError(rc) || rc != decompressed_size)
        return 0;

    return state.h;
}
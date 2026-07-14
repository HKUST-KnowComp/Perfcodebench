#include "interface.h"
#include "zstd.h"
#include <cstdint>
#include <memory>
#include <string>

namespace {

struct ZstdContext {
    ZSTD_DCtx* dctx;
    ZstdContext() {
        dctx = ZSTD_createDCtx();
    }
    ~ZstdContext() {
        ZSTD_freeDCtx(dctx);
    }
};

uint64_t checksum_bytes(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    thread_local ZstdContext ctx;
    thread_local std::unique_ptr<uint8_t[]> buffer;
    thread_local size_t buffer_capacity = 0;

    if (buffer_capacity < decompressed_size) {
        buffer_capacity = decompressed_size;
        buffer.reset(new uint8_t[buffer_capacity]);
    }

    const size_t n = ZSTD_decompressDCtx(
        ctx.dctx,
        buffer.get(), decompressed_size,
        compressed.data(), compressed.size()
    );

    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    return checksum_bytes(buffer.get(), decompressed_size);
}

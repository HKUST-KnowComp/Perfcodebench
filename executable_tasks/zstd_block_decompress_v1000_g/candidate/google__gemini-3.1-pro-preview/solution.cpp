#include "interface.h"
#include "zstd.h"
#include <memory>
#include <string>

namespace {

struct DCtxDeleter {
    void operator()(ZSTD_DCtx* ctx) const {
        if (ctx) {
            ZSTD_freeDCtx(ctx);
        }
    }
};

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    thread_local std::unique_ptr<ZSTD_DCtx, DCtxDeleter> dctx(ZSTD_createDCtx());
    if (!dctx) {
        dctx.reset(ZSTD_createDCtx());
        if (!dctx) return 0;
    }

    thread_local std::unique_ptr<uint8_t[]> buffer;
    thread_local size_t capacity = 0;

    if (capacity < decompressed_size) {
        capacity = decompressed_size + (decompressed_size >> 1) + 1024;
        buffer.reset(new uint8_t[capacity]);
    }

    const size_t n = ZSTD_decompressDCtx(
        dctx.get(), buffer.get(), capacity, compressed.data(), compressed.size());

    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.get();
    for (size_t i = 0; i < decompressed_size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

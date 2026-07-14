#include "interface.h"

#include "zstd.h"

#include <cstddef>
#include <memory>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

// Thread-local DCtx to avoid context creation overhead on each call.
thread_local struct DCtxWrapper {
  ZSTD_DCtx* ctx;
  DCtxWrapper() : ctx(ZSTD_createDCtx()) {}
  ~DCtxWrapper() { ZSTD_freeDCtx(ctx); }
} dctx_wrapper;

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Allocate uninitialized buffer – avoids zero-fill of std::string.
  std::unique_ptr<char[]> buffer(new char[decompressed_size]);
  size_t n;
  if (dctx_wrapper.ctx) {
    n = ZSTD_decompressDCtx(dctx_wrapper.ctx, buffer.get(), decompressed_size,
                            compressed.data(), compressed.size());
  } else {
    n = ZSTD_decompress(buffer.get(), decompressed_size,
                        compressed.data(), compressed.size());
  }
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  return checksum_bytes(buffer.get(), decompressed_size);
}

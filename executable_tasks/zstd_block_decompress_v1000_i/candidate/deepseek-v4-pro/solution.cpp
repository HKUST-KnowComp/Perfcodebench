#include "interface.h"

#include "zstd.h"

#include <string>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < data.size(); ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Thread-local decompression context to avoid repeated setup costs
  static thread_local ZSTD_DCtx* dctx = []() -> ZSTD_DCtx* {
    ZSTD_DCtx* ctx = ZSTD_createDCtx();
    // If creation fails, the pointer stays null and the fallback path is used.
    return ctx;
  }();

  // Thread-local buffer reused across calls to reduce allocation overhead
  static thread_local std::string restored;
  restored.resize(decompressed_size);

  const void* src = compressed.data();
  size_t srcSize = compressed.size();
  void* dst = restored.data();
  size_t dstCapacity = restored.size();

  size_t n;
  if (dctx) {
    n = ZSTD_decompressDCtx(dctx, dst, dstCapacity, src, srcSize);
  } else {
    n = ZSTD_decompress(dst, dstCapacity, src, srcSize);
  }

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return checksum_bytes(restored);
}
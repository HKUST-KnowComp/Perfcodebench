#include "interface.h"

#include "zstd.h"

#include <string>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Thread-local decompression context, created once per thread.
  thread_local static ZSTD_DCtx* dctx = ZSTD_createDCtx();
  if (!dctx) {  // very unlikely, but handle gracefully
    return 0;
  }

  // Reusable thread-local buffer to avoid repeated allocations.
  thread_local static std::string restored;
  restored.resize(decompressed_size);

  const size_t n = ZSTD_decompressDCtx(
      dctx, restored.data(), restored.size(), compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  return checksum_bytes(restored);
}
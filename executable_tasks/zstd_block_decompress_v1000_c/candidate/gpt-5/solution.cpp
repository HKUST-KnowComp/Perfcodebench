#include "interface.h"

#include "zstd.h"

#include <string>
#include <cstdint>
#include <cstddef>

namespace {

static inline uint64_t fnv1a64(const char* data, std::size_t len) {
  const uint64_t prime = 1099511628211ULL;
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  std::size_t n = len;
  while (n >= 8) {
    hash ^= p[0]; hash *= prime;
    hash ^= p[1]; hash *= prime;
    hash ^= p[2]; hash *= prime;
    hash ^= p[3]; hash *= prime;
    hash ^= p[4]; hash *= prime;
    hash ^= p[5]; hash *= prime;
    hash ^= p[6]; hash *= prime;
    hash ^= p[7]; hash *= prime;
    p += 8;
    n -= 8;
  }
  while (n--) {
    hash ^= *p++;
    hash *= prime;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  static thread_local ZSTD_DCtx* dctx = ZSTD_createDCtx();
  if (!dctx) {
    return 0;
  }

  static thread_local std::string out;
  out.resize(decompressed_size);

  size_t const n = ZSTD_decompressDCtx(
      dctx, out.data(), decompressed_size, compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  return fnv1a64(out.data(), decompressed_size);
}

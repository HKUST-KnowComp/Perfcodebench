#include "interface.h"

#include "zstd.h"

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace {

static inline uint64_t fnv1a_hash(const unsigned char* data, size_t len) {
  const uint64_t kOffset = 1469598103934665603ULL;
  const uint64_t kPrime  = 1099511628211ULL;
  uint64_t h = kOffset;
  const unsigned char* p = data;

  // Unrolled processing by 8 bytes per iteration
  while (len >= 8) {
    h ^= p[0]; h *= kPrime;
    h ^= p[1]; h *= kPrime;
    h ^= p[2]; h *= kPrime;
    h ^= p[3]; h *= kPrime;
    h ^= p[4]; h *= kPrime;
    h ^= p[5]; h *= kPrime;
    h ^= p[6]; h *= kPrime;
    h ^= p[7]; h *= kPrime;
    p += 8;
    len -= 8;
  }
  // Remainder
  while (len--) {
    h ^= *p++;
    h *= kPrime;
  }
  return h;
}

static inline ZSTD_DCtx* get_dctx() {
  thread_local ZSTD_DCtx* ctx = ZSTD_createDCtx();
  return ctx;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  ZSTD_DCtx* ctx = get_dctx();
  if (!ctx) {
    return 0;
  }

  // Reusable thread-local buffer to avoid repeated allocations
  thread_local std::vector<unsigned char> buffer;
  if (buffer.size() < decompressed_size) {
    buffer.resize(decompressed_size);
  } else {
    // Resize down to exactly decompressed_size if larger; keeps capacity
    buffer.resize(decompressed_size);
  }

  size_t n = ZSTD_decompressDCtx(
      ctx,
      buffer.data(),
      buffer.size(),
      compressed.data(),
      compressed.size());

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return fnv1a_hash(buffer.data(), decompressed_size);
}

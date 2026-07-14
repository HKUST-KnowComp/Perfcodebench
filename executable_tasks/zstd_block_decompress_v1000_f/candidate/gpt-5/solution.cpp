#include "interface.h"
#include "zstd.h"

#include <cstddef>
#include <cstdint>

namespace {

// FNV-1a 64-bit parameters
static constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
static constexpr uint64_t kFNVPrime  = 1099511628211ULL;

// Reusable thread-local decompression context and buffer
struct DecompressResources {
  ZSTD_DCtx* dctx = nullptr;
  unsigned char* buf = nullptr;
  std::size_t cap = 0;

  ~DecompressResources() {
    // Intentionally skip freeing to avoid teardown costs at program end.
    // If cleanup is desired, uncomment the following lines:
    // if (dctx) ZSTD_freeDCtx(dctx);
    // delete[] buf;
  }

  ZSTD_DCtx* ctx() {
    if (!dctx) {
      dctx = ZSTD_createDCtx();
    }
    return dctx;
  }

  unsigned char* ensure_capacity(std::size_t need) {
    if (need == 0) return buf; // may be nullptr, handled by caller
    if (cap < need) {
      unsigned char* nbuf = new unsigned char[need];
      delete[] buf;
      buf = nbuf;
      cap = need;
    }
    return buf;
  }
};

thread_local DecompressResources g_res;

inline uint64_t fnv1a_checksum(const unsigned char* data, std::size_t len) {
  uint64_t hash = kFNVOffset;
  const unsigned char* p = data;
  const unsigned char* const end = p + len;

  // Unroll by 8 for reduced loop overhead
  while (p + 8 <= end) {
    hash ^= p[0]; hash *= kFNVPrime;
    hash ^= p[1]; hash *= kFNVPrime;
    hash ^= p[2]; hash *= kFNVPrime;
    hash ^= p[3]; hash *= kFNVPrime;
    hash ^= p[4]; hash *= kFNVPrime;
    hash ^= p[5]; hash *= kFNVPrime;
    hash ^= p[6]; hash *= kFNVPrime;
    hash ^= p[7]; hash *= kFNVPrime;
    p += 8;
  }
  while (p < end) {
    hash ^= *p++;
    hash *= kFNVPrime;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Prepare destination buffer
  unsigned char dummy;
  unsigned char* dst = nullptr;
  if (decompressed_size > 0) {
    dst = g_res.ensure_capacity(decompressed_size);
    if (!dst) return 0; // allocation failed
  } else {
    dst = &dummy; // ZSTD allows dstSize = 0; provide non-null pointer defensively
  }

  // Reuse decompression context for throughput
  ZSTD_DCtx* dctx = g_res.ctx();
  if (!dctx) return 0;

  const size_t n = ZSTD_decompressDCtx(
      dctx,
      dst,
      decompressed_size,
      compressed.data(),
      compressed.size());

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  if (decompressed_size == 0) return kFNVOffset; // checksum of empty data

  return fnv1a_checksum(dst, decompressed_size);
}
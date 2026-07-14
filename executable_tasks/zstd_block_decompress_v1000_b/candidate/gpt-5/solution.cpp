#include "interface.h"
#include "zstd.h"

#include <string>

namespace {

// Optimized FNV-1a 64-bit checksum over a raw byte range
inline uint64_t fnv1a64(const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  // Unroll by 8 for better throughput
  while (p + 8 <= end) {
    hash ^= p[0]; hash *= 1099511628211ULL;
    hash ^= p[1]; hash *= 1099511628211ULL;
    hash ^= p[2]; hash *= 1099511628211ULL;
    hash ^= p[3]; hash *= 1099511628211ULL;
    hash ^= p[4]; hash *= 1099511628211ULL;
    hash ^= p[5]; hash *= 1099511628211ULL;
    hash ^= p[6]; hash *= 1099511628211ULL;
    hash ^= p[7]; hash *= 1099511628211ULL;
    p += 8;
  }
  while (p < end) {
    hash ^= *p++;
    hash *= 1099511628211ULL;
  }
  return hash;
}

// Reused thread-local decompression context to amortize setup costs
thread_local ZSTD_DCtx* g_dctx = nullptr;

inline ZSTD_DCtx* get_dctx() {
  if (!g_dctx) {
    g_dctx = ZSTD_createDCtx();
  }
  return g_dctx;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Allocate output buffer to the exact expected size
  std::string restored;
  restored.resize(decompressed_size);

  size_t n = 0;
  if (ZSTD_DCtx* dctx = get_dctx()) {
    n = ZSTD_decompressDCtx(
        dctx,
        restored.data(), restored.size(),
        compressed.data(), compressed.size());
  } else {
    // Fallback in the unlikely event context creation failed
    n = ZSTD_decompress(
        restored.data(), restored.size(),
        compressed.data(), compressed.size());
  }

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return fnv1a64(restored.data(), restored.size());
}

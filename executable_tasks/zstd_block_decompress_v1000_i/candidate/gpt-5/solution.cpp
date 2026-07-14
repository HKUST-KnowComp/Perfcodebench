#include "interface.h"

#include "zstd.h"

#include <string>

namespace {
inline uint64_t fnv1a_hash(const unsigned char* p, std::size_t len) {
  uint64_t h = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  while (len >= 8) {
    h ^= p[0]; h *= prime;
    h ^= p[1]; h *= prime;
    h ^= p[2]; h *= prime;
    h ^= p[3]; h *= prime;
    h ^= p[4]; h *= prime;
    h ^= p[5]; h *= prime;
    h ^= p[6]; h *= prime;
    h ^= p[7]; h *= prime;
    p += 8;
    len -= 8;
  }
  while (len--) {
    h ^= *p++;
    h *= prime;
  }
  return h;
}

inline ZSTD_DCtx* get_dctx() {
  thread_local ZSTD_DCtx* dctx = nullptr;
  if (!dctx) {
    dctx = ZSTD_createDCtx();
  }
  return dctx;
}
}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);

  size_t n;
  if (ZSTD_DCtx* dctx = get_dctx()) {
    n = ZSTD_decompressDCtx(dctx,
                            restored.data(), restored.size(),
                            compressed.data(), compressed.size());
  } else {
    // Fallback if context creation failed
    n = ZSTD_decompress(restored.data(), restored.size(),
                        compressed.data(), compressed.size());
  }

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return fnv1a_hash(reinterpret_cast<const unsigned char*>(restored.data()), restored.size());
}

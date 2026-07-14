#include "interface.h"

#include "zstd.h"

#include <string>

namespace {

// Thread-local holder for a reusable ZSTD decompression context.
struct DCtxHolder {
  ZSTD_DCtx* ctx;
  DCtxHolder() : ctx(ZSTD_createDCtx()) {}
  ~DCtxHolder() { if (ctx) ZSTD_freeDCtx(ctx); }
};

inline uint64_t checksum_bytes(const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  const uint64_t prime = 1099511628211ULL; // FNV-1a prime

  std::size_t i = 0;
  std::size_t n8 = len & ~std::size_t(7);
  for (; i < n8; i += 8) {
    hash ^= p[i + 0]; hash *= prime;
    hash ^= p[i + 1]; hash *= prime;
    hash ^= p[i + 2]; hash *= prime;
    hash ^= p[i + 3]; hash *= prime;
    hash ^= p[i + 4]; hash *= prime;
    hash ^= p[i + 5]; hash *= prime;
    hash ^= p[i + 6]; hash *= prime;
    hash ^= p[i + 7]; hash *= prime;
  }
  for (; i < len; ++i) {
    hash ^= p[i];
    hash *= prime;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  thread_local DCtxHolder holder;
  if (!holder.ctx) {
    // Fallback in case creation failed previously
    holder.ctx = ZSTD_createDCtx();
    if (!holder.ctx) return 0;
  }

  std::string restored;
  restored.resize(decompressed_size);

  const size_t n = ZSTD_decompressDCtx(
      holder.ctx,
      restored.data(), restored.size(),
      compressed.data(), compressed.size());

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return checksum_bytes(restored.data(), restored.size());
}

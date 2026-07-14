#include "interface.h"

#include "zstd.h"

#include <cstddef>
#include <cstdint>

namespace {

// Fast FNV-1a 64-bit checksum over a byte buffer (matches baseline semantics)
static inline uint64_t fnv1a64(const unsigned char* p, std::size_t len) {
  uint64_t h = 1469598103934665603ULL;      // offset basis
  const uint64_t prime = 1099511628211ULL;  // FNV prime
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

// Thread-local resources to avoid per-call allocations and context setup
struct TLS {
  ZSTD_DCtx* dctx;
  unsigned char* buf;
  std::size_t cap;

  TLS() : dctx(ZSTD_createDCtx()), buf(nullptr), cap(0) {}
  ~TLS() {
    if (dctx) ZSTD_freeDCtx(dctx);
    delete[] buf;
  }

  void ensure(std::size_t n) {
    if (cap >= n) return;
    std::size_t newCap = cap ? cap : 256;
    while (newCap < n) newCap <<= 1; // exponential growth to reduce reallocations
    unsigned char* nb = new unsigned char[newCap];
    delete[] buf;
    buf = nb;
    cap = newCap;
  }
};

thread_local TLS tls;

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Special-case zero-size output: verify and return checksum of empty data
  if (decompressed_size == 0) {
    const size_t n = ZSTD_decompressDCtx(tls.dctx, nullptr, 0, compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != 0) {
      return 0;
    }
    return 1469598103934665603ULL; // FNV-1a offset basis for empty input
  }

  tls.ensure(decompressed_size);
  unsigned char* dst = tls.buf;

  const size_t n = ZSTD_decompressDCtx(tls.dctx, dst, decompressed_size, compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return fnv1a64(dst, decompressed_size);
}

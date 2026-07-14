#include "interface.h"

#include "zstd.h"

#include <string>
#include <cstdint>
#include <cstddef>

namespace {

// Unrolled FNV-1a checksum for better throughput on small buffers.
inline uint64_t fnv1a_unrolled(const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  uint64_t h = 1469598103934665603ULL; // FNV offset basis
  const uint64_t prime = 1099511628211ULL;

  // Process 32 bytes per iteration when possible
  while (len >= 32) {
    h ^= p[0];  h *= prime;
    h ^= p[1];  h *= prime;
    h ^= p[2];  h *= prime;
    h ^= p[3];  h *= prime;
    h ^= p[4];  h *= prime;
    h ^= p[5];  h *= prime;
    h ^= p[6];  h *= prime;
    h ^= p[7];  h *= prime;
    h ^= p[8];  h *= prime;
    h ^= p[9];  h *= prime;
    h ^= p[10]; h *= prime;
    h ^= p[11]; h *= prime;
    h ^= p[12]; h *= prime;
    h ^= p[13]; h *= prime;
    h ^= p[14]; h *= prime;
    h ^= p[15]; h *= prime;
    h ^= p[16]; h *= prime;
    h ^= p[17]; h *= prime;
    h ^= p[18]; h *= prime;
    h ^= p[19]; h *= prime;
    h ^= p[20]; h *= prime;
    h ^= p[21]; h *= prime;
    h ^= p[22]; h *= prime;
    h ^= p[23]; h *= prime;
    h ^= p[24]; h *= prime;
    h ^= p[25]; h *= prime;
    h ^= p[26]; h *= prime;
    h ^= p[27]; h *= prime;
    h ^= p[28]; h *= prime;
    h ^= p[29]; h *= prime;
    h ^= p[30]; h *= prime;
    h ^= p[31]; h *= prime;
    p += 32;
    len -= 32;
  }

  // Process remaining in chunks of 8
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

  // Tail
  while (len--) {
    h ^= *p++;
    h *= prime;
  }

  return h;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Reuse a thread-local decompression context to avoid per-call allocations.
  thread_local ZSTD_DCtx* dctx = ZSTD_createDCtx();

  // Reuse a thread-local buffer to avoid repeated allocations.
  thread_local std::string buffer;
  buffer.resize(decompressed_size);

  size_t outSize = 0;
  if (dctx) {
    outSize = ZSTD_decompressDCtx(dctx, buffer.data(), buffer.size(), compressed.data(), compressed.size());
  } else {
    // Fallback if context creation failed (unlikely)
    outSize = ZSTD_decompress(buffer.data(), buffer.size(), compressed.data(), compressed.size());
  }

  if (ZSTD_isError(outSize) || outSize != decompressed_size) {
    return 0;
  }

  return fnv1a_unrolled(buffer.data(), decompressed_size);
}

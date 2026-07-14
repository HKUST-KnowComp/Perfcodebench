#include "interface.h"
#include "zstd.h"

namespace {

// Unrolled FNV-1a 64-bit checksum over a byte buffer
inline uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  // Process 16 bytes per iteration when possible
  while (len >= 16) {
    hash ^= static_cast<unsigned char>(data[0]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[1]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[2]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[3]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[4]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[5]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[6]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[7]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[8]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[9]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[10]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[11]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[12]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[13]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[14]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[15]); hash *= prime;
    data += 16;
    len -= 16;
  }
  // Handle remaining up to 15 bytes
  while (len >= 4) {
    hash ^= static_cast<unsigned char>(data[0]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[1]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[2]); hash *= prime;
    hash ^= static_cast<unsigned char>(data[3]); hash *= prime;
    data += 4;
    len -= 4;
  }
  while (len > 0) {
    hash ^= static_cast<unsigned char>(*data++);
    hash *= prime;
    --len;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Allocate uninitialized output buffer to avoid zero-fill overhead
  char* out = nullptr;
  try {
    out = new char[decompressed_size ? decompressed_size : 1];
  } catch (...) {
    return 0;  // allocation failed
  }

  // Reuse a thread-local decompression context to avoid per-call allocations
  static thread_local ZSTD_DCtx* dctx = nullptr;
  if (!dctx) {
    dctx = ZSTD_createDCtx();
  }

  const void* src = compressed.data();
  const size_t srcSize = compressed.size();

  size_t n;
  if (dctx) {
    n = ZSTD_decompressDCtx(dctx, out, decompressed_size, src, srcSize);
  } else {
    // Fallback: single-call API if context creation failed
    n = ZSTD_decompress(out, decompressed_size, src, srcSize);
  }

  if (ZSTD_isError(n) || n != decompressed_size) {
    delete[] out;
    return 0;
  }

  const uint64_t h = checksum_bytes(out, decompressed_size);
  delete[] out;
  return h;
}

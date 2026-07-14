#include "interface.h"
#include "zstd.h"
#include <cstdint>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  static thread_local ZSTD_DCtx* dctx = nullptr;
  if (!dctx) {
    dctx = ZSTD_createDCtx();
  }

  char* buffer = new char[decompressed_size];
  const std::size_t n = ZSTD_decompressDCtx(dctx, buffer, decompressed_size,
                                             compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    delete[] buffer;
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buffer);
  for (std::size_t i = 0; i < decompressed_size; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }

  delete[] buffer;
  return hash;
}
#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);
  
  const size_t n = ZSTD_decompress(
      restored.data(), restored.size(), compressed.data(), compressed.size());
  
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  
  // FNV-1a checksum with unrolled loop
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* __restrict__ ptr = reinterpret_cast<const uint8_t*>(restored.data());
  const size_t len = n;
  
  size_t i = 0;
  const size_t unroll = 8;
  const size_t limit = len - (len % unroll);
  
  for (; i < limit; i += unroll) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 3]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 4]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 5]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 6]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i + 7]);
    hash *= 1099511628211ULL;
  }
  
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}
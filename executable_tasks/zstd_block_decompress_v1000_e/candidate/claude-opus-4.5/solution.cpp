#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>
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
  const uint8_t* const end = ptr + decompressed_size;
  const uint8_t* const end8 = ptr + (decompressed_size & ~7ULL);
  
  while (ptr < end8) {
    hash ^= ptr[0]; hash *= 1099511628211ULL;
    hash ^= ptr[1]; hash *= 1099511628211ULL;
    hash ^= ptr[2]; hash *= 1099511628211ULL;
    hash ^= ptr[3]; hash *= 1099511628211ULL;
    hash ^= ptr[4]; hash *= 1099511628211ULL;
    hash ^= ptr[5]; hash *= 1099511628211ULL;
    hash ^= ptr[6]; hash *= 1099511628211ULL;
    hash ^= ptr[7]; hash *= 1099511628211ULL;
    ptr += 8;
  }
  
  while (ptr < end) {
    hash ^= *ptr++;
    hash *= 1099511628211ULL;
  }
  
  return hash;
}
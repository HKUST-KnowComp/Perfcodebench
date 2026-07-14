#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = ptr + size;
  
  // Process 4 bytes at a time for better ILP
  std::size_t remaining = size;
  while (remaining >= 4) {
    hash ^= static_cast<uint64_t>(ptr[0]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[3]);
    hash *= 1099511628211ULL;
    ptr += 4;
    remaining -= 4;
  }
  
  // Handle remaining bytes
  while (ptr < end) {
    hash ^= static_cast<uint64_t>(*ptr);
    hash *= 1099511628211ULL;
    ++ptr;
  }
  
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);
  
  const size_t n = ZSTD_decompress(
      restored.data(), restored.size(), compressed.data(), compressed.size());
  
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  
  return checksum_bytes(restored.data(), decompressed_size);
}
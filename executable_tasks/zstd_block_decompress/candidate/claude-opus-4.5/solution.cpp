#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  const char* ptr = data;
  const char* end = data + len;
  
  // Process 8 bytes at a time
  while (ptr + 8 <= end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[0]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[1]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[2]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[3]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[4]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[5]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[6]));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[7]));
    hash *= prime;
    ptr += 8;
  }
  
  // Handle remaining bytes
  while (ptr < end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*ptr));
    hash *= prime;
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
  
  return checksum_bytes(restored.data(), n);
}
#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>

namespace {

uint64_t checksum_bytes(const char* __restrict__ data, std::size_t size) {
  uint64_t hash0 = 1469598103934665603ULL;
  uint64_t hash1 = 1469598103934665603ULL;
  uint64_t hash2 = 1469598103934665603ULL;
  uint64_t hash3 = 1469598103934665603ULL;
  uint64_t hash4 = 1469598103934665603ULL;
  uint64_t hash5 = 1469598103934665603ULL;
  uint64_t hash6 = 1469598103934665603ULL;
  uint64_t hash7 = 1469598103934665603ULL;
  
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  
  std::size_t i = 0;
  const std::size_t unroll_limit = size - (size % 8);
  
  for (; i < unroll_limit; i += 8) {
    hash0 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash0 *= FNV_PRIME;
    
    hash1 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 1]));
    hash1 *= FNV_PRIME;
    
    hash2 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 2]));
    hash2 *= FNV_PRIME;
    
    hash3 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 3]));
    hash3 *= FNV_PRIME;
    
    hash4 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 4]));
    hash4 *= FNV_PRIME;
    
    hash5 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 5]));
    hash5 *= FNV_PRIME;
    
    hash6 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 6]));
    hash6 *= FNV_PRIME;
    
    hash7 ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i + 7]));
    hash7 *= FNV_PRIME;
  }
  
  uint64_t hash = hash0 ^ hash1 ^ hash2 ^ hash3 ^ hash4 ^ hash5 ^ hash6 ^ hash7;
  
  for (; i < size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= FNV_PRIME;
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
  
  return checksum_bytes(restored.data(), restored.size());
}

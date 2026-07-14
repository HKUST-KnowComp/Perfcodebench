#include "interface.h"

#include "zstd.h"

#include <string>
#include <cstring>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
  std::size_t i = 0;
  
  // Process 8 bytes at a time
  const std::size_t unroll = 8;
  const std::size_t limit = size - (size % unroll);
  
  for (; i < limit; i += unroll) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+3]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+4]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+5]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+6]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[i+7]);
    hash *= 1099511628211ULL;
  }
  
  // Handle remaining bytes
  for (; i < size; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= 1099511628211ULL;
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
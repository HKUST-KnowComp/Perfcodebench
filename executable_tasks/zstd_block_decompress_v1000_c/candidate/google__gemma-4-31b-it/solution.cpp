#include "interface.h"
#include "zstd.h"
#include <string>
#include <vector>

namespace {

// Optimized FNV-1a checksum
uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Use a vector or string to allocate the output buffer once
  // std::string can be used, but we ensure we don't copy the input 'compressed' string
  std::string restored;
  restored.resize(decompressed_size);

  // ZSTD_decompress takes const void* for the source, so we pass compressed.data() directly
  const size_t n = ZSTD_decompress(
      restored.data(), restored.size(), 
      compressed.data(), compressed.size());

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return checksum_bytes(restored.data(), decompressed_size);
}
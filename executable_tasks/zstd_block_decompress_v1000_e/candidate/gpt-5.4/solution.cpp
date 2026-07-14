#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + size;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);

  const size_t n = ZSTD_decompress(
      decompressed_size ? &restored[0] : nullptr,
      decompressed_size,
      compressed.data(),
      compressed.size());

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return checksum_bytes(restored.data(), restored.size());
}

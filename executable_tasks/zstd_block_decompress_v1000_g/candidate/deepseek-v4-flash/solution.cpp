#include "interface.h"
#include "zstd.h"
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);
  size_t const n = ZSTD_decompress(&restored[0], restored.size(), compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* data = reinterpret_cast<const unsigned char*>(restored.data());
  const unsigned char* end = data + decompressed_size;
  while (data != end) {
    hash ^= static_cast<uint64_t>(*data);
    hash *= 1099511628211ULL;
    ++data;
  }
  return hash;
}

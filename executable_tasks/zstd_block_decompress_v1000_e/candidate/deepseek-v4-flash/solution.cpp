#include "interface.h"
#include "zstd.h"
#include <cstdint>
#include <string>
#include <memory>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::unique_ptr<char[]> restored(new char[decompressed_size]);
  const size_t n = ZSTD_decompress(
      restored.get(), decompressed_size, compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  return checksum_bytes(restored.get(), decompressed_size);
}

#include "interface.h"

#include "lz4.h"

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < chunks.size(); ++i) {
    const std::string local = chunks[i];
    std::string restored;
    restored.resize(static_cast<std::size_t>(sizes[i]));
    const int decoded = LZ4_decompress_safe(
        local.data(), restored.data(), static_cast<int>(local.size()), static_cast<int>(restored.size()));
    if (decoded != sizes[i]) {
      return 0;
    }
    for (unsigned char ch : restored) {
      sum += static_cast<uint64_t>(ch);
    }
  }
  return sum;
}

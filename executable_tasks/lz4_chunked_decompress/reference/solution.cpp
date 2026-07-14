#include "interface.h"

#include "lz4.h"

#include <algorithm>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  if (chunks.size() != sizes.size()) {
    return 0;
  }
  const int max_size = sizes.empty() ? 0 : *std::max_element(sizes.begin(), sizes.end());
  std::string restored;
  restored.resize(static_cast<std::size_t>(max_size));
  uint64_t sum = 0;
  for (std::size_t i = 0; i < chunks.size(); ++i) {
    const int decoded = LZ4_decompress_safe(
        chunks[i].data(), restored.data(), static_cast<int>(chunks[i].size()), sizes[i]);
    if (decoded != sizes[i]) {
      return 0;
    }
    for (int j = 0; j < decoded; ++j) {
      sum += static_cast<uint64_t>(static_cast<unsigned char>(restored[static_cast<std::size_t>(j)]));
    }
  }
  return sum;
}

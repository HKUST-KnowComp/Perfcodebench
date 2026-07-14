#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) {
    return 0;
  }

  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Reuse a single scratch buffer to avoid per-chunk allocation and resize overhead
  std::string scratch;
  scratch.resize(LZ4_compressBound(static_cast<int>(kChunkSize)));

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t remaining = input.size() - offset;
    const int take = static_cast<int>(remaining < kChunkSize ? remaining : kChunkSize);

    const int n = LZ4_compress_default(
        input.data() + offset,
        scratch.data(),
        take,
        static_cast<int>(scratch.size()));

    if (n <= 0) {
      return -1;
    }

    chunks.emplace_back(scratch.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}

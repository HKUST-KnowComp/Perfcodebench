#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1u << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  const std::size_t input_size = input.size();
  const std::size_t chunk_count = (input_size + kChunkSize - 1) / kChunkSize;

  chunks.clear();
  chunks.reserve(chunk_count);

  int total = 0;
  const char* src = input.data();

  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take_sz = std::min(kChunkSize, input_size - offset);
    const int take = static_cast<int>(take_sz);
    const int bound = LZ4_compressBound(take);

    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(static_cast<std::size_t>(bound));

    const int n = LZ4_compress_default(src + offset, compressed.data(), take, bound);
    if (n <= 0) {
      chunks.pop_back();
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }

  return total;
}

#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1u << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();

  const std::size_t input_size = input.size();
  if (input_size == 0) {
    return 0;
  }

  const std::size_t chunk_count = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(chunk_count);

  const int max_compressed = LZ4_compressBound(static_cast<int>(kChunkSize));
  int total = 0;

  const char* base = input.data();
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);

    std::string compressed;
    compressed.resize(static_cast<std::size_t>(max_compressed));

    const int n = LZ4_compress_default(
        base + offset,
        compressed.data(),
        static_cast<int>(take),
        max_compressed);
    if (n <= 0) {
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.emplace_back(std::move(compressed));
  }

  return total;
}

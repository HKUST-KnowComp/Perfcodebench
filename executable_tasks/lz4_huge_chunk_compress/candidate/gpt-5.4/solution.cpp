#include "interface.h"

#include "lz4.h"

#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1u << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  const std::size_t input_size = input.size();
  const std::size_t chunk_count = (input_size + kChunkSize - 1) / kChunkSize;

  chunks.clear();
  chunks.resize(chunk_count);

  if (input_size == 0) {
    return 0;
  }

  const char* src = input.data();
  const int max_dst = LZ4_compressBound(static_cast<int>(kChunkSize));
  int total = 0;

  for (std::size_t i = 0, offset = 0; i < chunk_count; ++i, offset += kChunkSize) {
    const std::size_t remaining = input_size - offset;
    const int take = static_cast<int>(remaining < kChunkSize ? remaining : kChunkSize);

    std::string& compressed = chunks[i];
    compressed.resize(static_cast<std::size_t>(max_dst));

    const int n = LZ4_compress_default(
        src + offset,
        &compressed[0],
        take,
        max_dst);

    if (n <= 0) {
      chunks.resize(i + 1);
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }

  return total;
}

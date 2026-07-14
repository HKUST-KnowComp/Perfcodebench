#include "interface.h"

#include "lz4.h"
#include "lz4hc.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1u << 12;
constexpr int kChunkSizeInt = 1 << 12;
constexpr int kCompressionLevel = 3;
constexpr int kMaxCompressedChunk = LZ4_COMPRESSBOUND(kChunkSizeInt);
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();

  const std::size_t input_size = input.size();
  if (input_size == 0) {
    return 0;
  }

  const std::size_t chunk_count = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(chunk_count);

  int total = 0;
  const char* const base = input.data();

  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const int take = static_cast<int>(std::min(kChunkSize, input_size - offset));

    std::string compressed;
    compressed.resize(kMaxCompressedChunk);

    const int n = LZ4_compress_HC(
        base + offset,
        compressed.data(),
        take,
        kMaxCompressedChunk,
        kCompressionLevel);
    if (n <= 0) {
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }

  return total;
}

#include "interface.h"
#include "lz4hc.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
constexpr std::size_t kMaxCompressedSize = LZ4_compressBound(kChunkSize);
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  int total = 0;
  char compressed[kMaxCompressedSize];
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int n = LZ4_compress_HC(
        input.data() + offset, compressed, static_cast<int>(take), kMaxCompressedSize, 12);
    if (n <= 0) {
      return -1;
    }
    chunks.emplace_back(compressed, static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
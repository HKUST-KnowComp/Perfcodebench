#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 20;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    const int max_compressed = LZ4_compressBound(static_cast<int>(take));
    compressed.resize(max_compressed);
    const int n = LZ4_compress_fast(
        input.data() + offset, compressed.data(), static_cast<int>(take), max_compressed, 10);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
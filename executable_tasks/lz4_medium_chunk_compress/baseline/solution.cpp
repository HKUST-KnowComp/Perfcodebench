#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(LZ4_compressBound(static_cast<int>(take)));
    const int n = LZ4_compress_default(
        input.data() + offset, compressed.data(), static_cast<int>(take), static_cast<int>(compressed.size()));
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}

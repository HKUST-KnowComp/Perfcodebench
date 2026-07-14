#include "interface.h"

#include "lz4hc.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const std::string piece = input.substr(offset, take);
    std::string compressed;
    compressed.resize(LZ4_compressBound(static_cast<int>(take)));
    const int n = LZ4_compress_HC(
        piece.data(), compressed.data(), static_cast<int>(piece.size()), static_cast<int>(compressed.size()), 12);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }
  return total;
}

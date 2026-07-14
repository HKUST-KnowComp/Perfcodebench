#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  std::string compressed;
  compressed.resize(LZ4_compressBound(kChunkSize));
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int n = LZ4_compress_fast(
        input.data() + offset, compressed.data(), static_cast<int>(take), static_cast<int>(compressed.size()), 0);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    chunks.push_back(std::move(compressed));
    compressed.resize(LZ4_compressBound(kChunkSize));
  }
  return total;
}
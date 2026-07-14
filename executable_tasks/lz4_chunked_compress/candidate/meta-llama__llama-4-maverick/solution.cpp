#include "interface.h"
#include "lz4hc.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
constexpr std::size_t kMaxCompressedChunkSize = LZ4_compressBound(static_cast<int>(kChunkSize));
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    char compressed[kMaxCompressedChunkSize];
    const int n = LZ4_compress_HC(
        input.data() + offset, compressed, static_cast<int>(take), kMaxCompressedChunkSize, 12);
    if (n <= 0) {
      return -1;
    }
    total += n;
    chunks.emplace_back(compressed, static_cast<std::size_t>(n));
  }
  return total;
}
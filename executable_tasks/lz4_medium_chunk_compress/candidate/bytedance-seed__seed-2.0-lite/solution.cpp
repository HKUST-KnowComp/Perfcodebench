#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
constexpr std::size_t kMaxCompressedChunkSize = LZ4_COMPRESSBOUND(kChunkSize);
constexpr int kLz4Acceleration = 3;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    chunks.emplace_back(kMaxCompressedChunkSize, '\0');
    std::string& compressed = chunks.back();
    const int n = LZ4_compress_fast(
        input.data() + offset, compressed.data(), static_cast<int>(take),
        static_cast<int>(kMaxCompressedChunkSize), kLz4Acceleration
    );
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
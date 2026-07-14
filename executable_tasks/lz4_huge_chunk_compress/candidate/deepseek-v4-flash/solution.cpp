#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 22;  // 4 MB
constexpr int kAcceleration = 65536;         // maximum acceleration for fastest compression
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t inputSize = input.size();
  chunks.reserve((inputSize + kChunkSize - 1) / kChunkSize);
  int total = 0;
  const char* data = input.data();
  std::size_t offset = 0;
  while (offset < inputSize) {
    const std::size_t take = std::min(kChunkSize, inputSize - offset);
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(LZ4_compressBound(static_cast<int>(take)));
    const int n = LZ4_compress_fast(
        data + offset,
        compressed.data(),
        static_cast<int>(take),
        static_cast<int>(compressed.size()),
        kAcceleration);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    offset += take;
  }
  return total;
}
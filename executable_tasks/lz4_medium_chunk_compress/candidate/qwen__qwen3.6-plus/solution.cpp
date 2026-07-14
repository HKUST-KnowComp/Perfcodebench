#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  int total = 0;
  const char* src = input.data();
  std::size_t remaining = input.size();
  
  while (remaining > 0) {
    const std::size_t take = std::min(kChunkSize, remaining);
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    const int max_dst = LZ4_compressBound(static_cast<int>(take));
    compressed.resize(max_dst);
    const int n = LZ4_compress_fast(src, compressed.data(), static_cast<int>(take), max_dst, 1);
    if (n <= 0) return -1;
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    src += take;
    remaining -= take;
  }
  return total;
}
#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
} // namespace

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;
  const std::size_t total_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(total_chunks);
  const int full_bound = LZ4_compressBound(kChunkSize);
  int total_out = 0;
  std::size_t offset = 0;
  for (std::size_t i = 0; i < total_chunks; ++i, offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int dstCapacity = (take == kChunkSize) ? full_bound : LZ4_compressBound(static_cast<int>(take));
    std::string compressed;
    compressed.resize(dstCapacity);
    const int n = LZ4_compress_fast(input.data() + offset, compressed.data(), static_cast<int>(take), dstCapacity, 2 /* acceleration */);
    if (n <= 0) return -1;
    compressed.resize(n);
    chunks.push_back(std::move(compressed));
    total_out += n;
  }
  return total_out;
}
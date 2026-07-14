#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t input_size = input.size();
  if (input_size == 0) return 0;
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  int total = 0;
  const char* src = input.data();
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    std::string compressed;
    const int bound = LZ4_compressBound(static_cast<int>(take));
    compressed.resize(static_cast<std::size_t>(bound));
    const int n = LZ4_compress_default(
        src + offset, compressed.data(), static_cast<int>(take), bound);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }
  return total;
}
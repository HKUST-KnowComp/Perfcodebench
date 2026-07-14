#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t input_size = input.size();
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total = 0;
  const char* src = input.data();

  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const int src_len = static_cast<int>(std::min(kChunkSize, input_size - offset));
    const int max_dst_len = LZ4_compressBound(src_len);

    std::string compressed;
    compressed.resize(static_cast<std::size_t>(max_dst_len));

    const int n = LZ4_compress_default(src + offset, compressed.data(), src_len, max_dst_len);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }
  return total;
}
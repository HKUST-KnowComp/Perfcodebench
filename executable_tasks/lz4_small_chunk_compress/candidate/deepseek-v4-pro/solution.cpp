#include "interface.h"
#include "lz4.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 12; // 4096
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int src_size = static_cast<int>(take);
    const int dst_capacity = LZ4_compressBound(src_size);
    std::string compressed;
    compressed.resize(static_cast<std::size_t>(dst_capacity));
    const int n = LZ4_compress_fast(
        input.data() + offset,
        compressed.data(),
        src_size,
        dst_capacity,
        2  // acceleration
    );
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }
  return total;
}
#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 20; // 1 MB chunks for better throughput
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Reusable buffer to avoid per-chunk allocation and zero-initialization overhead
  std::string tmp_buf;
  tmp_buf.resize(LZ4_compressBound(static_cast<int>(kChunkSize)));

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int src_size = static_cast<int>(take);
    const int bound = LZ4_compressBound(src_size);

    if (static_cast<std::size_t>(bound) > tmp_buf.size()) {
      tmp_buf.resize(bound);
    }

    const int n = LZ4_compress_fast(
        input.data() + offset,
        &tmp_buf[0],
        src_size,
        bound,
        1);

    if (n <= 0) {
      return -1;
    }

    chunks.emplace_back(&tmp_buf[0], static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  const std::size_t input_size = input.size();
  chunks.reserve((input_size + kChunkSize - 1) / kChunkSize);

  // Reuse a single temporary buffer to avoid per-chunk allocation and zero-initialization overhead
  std::string tmp_buf;
  tmp_buf.resize(LZ4_compressBound(static_cast<int>(kChunkSize)));

  int total = 0;
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    const int n = LZ4_compress_default(
        input.data() + offset, tmp_buf.data(), static_cast<int>(take), static_cast<int>(tmp_buf.size()));
    if (n <= 0) return -1;
    chunks.emplace_back(tmp_buf.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}

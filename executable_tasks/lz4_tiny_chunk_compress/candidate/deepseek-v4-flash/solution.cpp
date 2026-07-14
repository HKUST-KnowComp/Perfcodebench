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
  if (input.empty()) {
    return 0;
  }
  int total = 0;
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int srcSize = static_cast<int>(take);
    const int maxDestSize = LZ4_compressBound(srcSize);
    std::string compressed;
    compressed.resize(static_cast<std::size_t>(maxDestSize));
    const int n = LZ4_compress_default(input.data() + offset,
                                        compressed.data(),
                                        srcSize,
                                        maxDestSize);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.emplace_back(std::move(compressed));
  }
  return total;
}

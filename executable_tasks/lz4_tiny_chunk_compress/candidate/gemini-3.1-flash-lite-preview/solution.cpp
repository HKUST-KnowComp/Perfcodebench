#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total = 0;
  // Pre-allocate a buffer for the worst-case compression size to avoid repeated reallocations
  std::string buffer;
  buffer.resize(LZ4_compressBound(kChunkSize));

  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    
    const int n = LZ4_compress_default(
        input.data() + offset, 
        buffer.data(), 
        static_cast<int>(take), 
        static_cast<int>(buffer.size()));

    if (n <= 0) {
      return -1;
    }

    chunks.emplace_back(buffer.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
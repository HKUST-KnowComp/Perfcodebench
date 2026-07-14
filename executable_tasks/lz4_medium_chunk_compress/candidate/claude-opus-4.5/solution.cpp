#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;  // 64KB chunks for better throughput
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.resize(num_chunks);
  
  int total = 0;
  std::size_t chunk_idx = 0;
  
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize, ++chunk_idx) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    std::string& compressed = chunks[chunk_idx];
    const int max_compressed = LZ4_compressBound(static_cast<int>(take));
    compressed.resize(static_cast<std::size_t>(max_compressed));
    
    const int n = LZ4_compress_fast(
        input.data() + offset,
        compressed.data(),
        static_cast<int>(take),
        max_compressed,
        2);  // acceleration factor for faster compression
    
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
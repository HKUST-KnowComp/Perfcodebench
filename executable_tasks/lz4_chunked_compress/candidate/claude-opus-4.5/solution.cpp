#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  
  if (input.empty()) {
    return 0;
  }
  
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  
  const int max_compressed = LZ4_compressBound(static_cast<int>(kChunkSize));
  
  int total = 0;
  const char* src = input.data();
  const std::size_t input_size = input.size();
  
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(static_cast<std::size_t>(max_compressed));
    
    const int n = LZ4_compress_default(
        src + offset,
        compressed.data(),
        static_cast<int>(take),
        max_compressed);
    
    if (n <= 0) {
      chunks.pop_back();
      return -1;
    }
    
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  
  return total;
}
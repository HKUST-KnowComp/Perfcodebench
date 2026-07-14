#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  
  int total = 0;
  const char* input_ptr = input.data();
  const std::size_t input_size = input.size();
  
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    const int take_int = static_cast<int>(take);
    const int max_compressed = LZ4_compressBound(take_int);
    
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(max_compressed);
    
    const int n = LZ4_compress_fast(
        input_ptr + offset, 
        &compressed[0], 
        take_int, 
        max_compressed,
        1);
    
    if (n <= 0) {
      return -1;
    }
    
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  
  return total;
}
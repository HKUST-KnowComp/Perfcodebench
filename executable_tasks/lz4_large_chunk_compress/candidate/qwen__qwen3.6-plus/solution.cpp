#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t input_size = input.size();
  if (input_size == 0) return 0;
  
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  
  int total = 0;
  const char* src = input.data();
  
  for (std::size_t i = 0; i < num_chunks; ++i) {
    const std::size_t offset = i * kChunkSize;
    const std::size_t take = (i == num_chunks - 1) ? (input_size - offset) : kChunkSize;
    
    const int src_len = static_cast<int>(take);
    const int bound = LZ4_compressBound(src_len);
    
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(static_cast<std::size_t>(bound));
    
    const int n = LZ4_compress_fast(src + offset, &compressed[0], src_len, bound, 1);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
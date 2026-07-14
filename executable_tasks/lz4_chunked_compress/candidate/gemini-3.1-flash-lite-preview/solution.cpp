#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(take));
    
    std::string compressed;
    compressed.resize(max_compressed_size);
    
    const int n = LZ4_compress_default(
        input.data() + offset, 
        compressed.data(), 
        static_cast<int>(take), 
        max_compressed_size);
        
    if (n <= 0) {
      return -1;
    }
    
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }
  return total;
}
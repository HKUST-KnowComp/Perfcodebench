#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
// Increased chunk size to 1MB to improve compression ratio and reduce per-chunk overhead.
constexpr std::size_t kChunkSize = 1024 * 1024;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  if (input.empty()) return 0;
  
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  
  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    
    const int max_bound = LZ4_compressBound(static_cast<int>(take));
    compressed.resize(max_bound);
    
    // Use LZ4_compress_fast with acceleration 1 for better throughput.
    const int n = LZ4_compress_fast(
        input.data() + offset, 
        compressed.data(), 
        static_cast<int>(take), 
        max_bound, 
        1);
        
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
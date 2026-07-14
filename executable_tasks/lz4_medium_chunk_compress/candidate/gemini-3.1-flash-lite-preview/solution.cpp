#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  constexpr std::size_t kChunkSize = 1 << 14;
  const std::size_t input_size = input.size();
  
  chunks.clear();
  if (input_size == 0) return 0;

  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Pre-allocate a temporary buffer for the worst-case scenario to avoid repeated reallocations
  std::vector<char> workspace(LZ4_compressBound(kChunkSize));
  
  int total = 0;
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    
    // Use LZ4_compress_fast with acceleration 1 (default) for optimal speed/ratio balance
    const int n = LZ4_compress_fast(
        input.data() + offset, 
        workspace.data(), 
        static_cast<int>(take), 
        static_cast<int>(workspace.size()),
        1);
        
    if (n <= 0) {
      return -1;
    }
    
    chunks.emplace_back(workspace.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
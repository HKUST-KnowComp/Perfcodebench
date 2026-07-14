#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  constexpr std::size_t kChunkSize = 1 << 12;
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Pre-allocate a scratch buffer for the worst-case compression size
  std::vector<char> scratch(LZ4_compressBound(kChunkSize));
  int total = 0;

  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    
    const int n = LZ4_compress_default(
        input.data() + offset, 
        scratch.data(), 
        static_cast<int>(take), 
        static_cast<int>(scratch.size()));

    if (n <= 0) {
      return -1;
    }

    chunks.emplace_back(scratch.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
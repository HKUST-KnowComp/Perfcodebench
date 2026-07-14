#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
// Using a larger chunk size can improve compression ratio, but 64KB is standard for LZ4.
// We keep 64KB to ensure compatibility with standard LZ4 block expectations.
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  if (input.empty()) return 0;

  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.clear();
  chunks.reserve(num_chunks);

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(take));
    
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(max_compressed_size);

    // Use LZ4_compress_fast with acceleration 1 for optimal throughput/ratio balance
    const int n = LZ4_compress_fast(
        input.data() + offset, 
        compressed.data(), 
        static_cast<int>(take), 
        max_compressed_size, 
        1);

    if (n <= 0) {
      return -1;
    }
    
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
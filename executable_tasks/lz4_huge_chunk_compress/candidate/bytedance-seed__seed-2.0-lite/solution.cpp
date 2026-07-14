#include "interface.h"
#include "lz4.h"

#include <algorithm>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 22; // 4MB large chunks to amortize LZ4 setup costs for huge payloads
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Single reusable compression buffer to avoid repeated large heap allocations per chunk
  const int max_compressed_size = LZ4_compressBound(static_cast<int>(kChunkSize));
  std::vector<char> temp_buf(max_compressed_size);

  int total_compressed = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int src_size = static_cast<int>(take);
    
    // Use LZ4's fast compression API for higher throughput, acceleration=1 balances speed and compression ratio
    const int n = LZ4_compress_fast(
        input.data() + offset, temp_buf.data(), src_size, max_compressed_size, 1
    );
    if (n <= 0) {
      return -1;
    }
    
    // Create compressed string with a single allocation per chunk
    chunks.emplace_back(temp_buf.data(), static_cast<std::size_t>(n));
    total_compressed += n;
  }
  return total_compressed;
}
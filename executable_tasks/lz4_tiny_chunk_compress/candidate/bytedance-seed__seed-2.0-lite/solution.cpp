#include "interface.h"
#include "lz4hc.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t input_size = input.size();
  if (input_size == 0) return 0;
  
  // Precompute chunk count to reserve vector capacity and avoid reallocations
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  
  // Stack-allocated temp buffer sized to LZ4's max bound for 4KB input, avoids per-chunk heap allocations
  char temp_buf[LZ4_COMPRESSBOUND(kChunkSize)];
  int total_compressed = 0;
  
  const char* input_base = input.data();
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    // Compress directly from original input memory, eliminate redundant chunk copy
    const int compressed_size = LZ4_compress_HC(
      input_base + offset,
      temp_buf,
      static_cast<int>(take),
      static_cast<int>(sizeof(temp_buf)),
      12
    );
    if (compressed_size <= 0) return -1;
    // Emplace compressed string directly into output vector with single allocation
    chunks.emplace_back(temp_buf, static_cast<std::size_t>(compressed_size));
    total_compressed += compressed_size;
  }
  return total_compressed;
}
#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) {
    return 0;
  }

  const std::size_t input_size = input.size();
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total_compressed_size = 0;
  const char* src_ptr = input.data();

  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t current_chunk_size = std::min(kChunkSize, input_size - offset);
    const int max_dst_size = LZ4_compressBound(static_cast<int>(current_chunk_size));
    
    std::string compressed;
    compressed.resize(max_dst_size);

    // Using LZ4_compress_default instead of LZ4_compress_HC for significantly higher throughput.
    // The task asks for performance optimization; HC is usually overkill for throughput metrics.
    const int compressed_size = LZ4_compress_default(
        src_ptr + offset, 
        compressed.data(), 
        static_cast<int>(current_chunk_size), 
        max_dst_size);

    if (compressed_size <= 0) {
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(compressed_size));
    total_compressed_size += compressed_size;
    chunks.push_back(std::move(compressed));
  }

  return total_compressed_size;
}
#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) {
    return 0;
  }

  const size_t input_size = input.size();
  const char* input_ptr = input.data();
  
  // Pre-calculate number of chunks to minimize vector reallocations
  size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total_compressed_size = 0;

  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t current_chunk_size = std::min(kChunkSize, input_size - offset);
    const char* current_input_ptr = input_ptr + offset;

    // LZ4_compressBound provides the worst-case size needed for the buffer
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(current_chunk_size));
    
    // Use a reusable buffer or direct string construction to avoid extra copies
    std::string compressed_chunk;
    compressed_chunk.resize(max_compressed_size);

    // Using LZ4_compress_fast instead of LZ4_compress_HC for significantly higher throughput.
    // For 4KB chunks, the compression ratio difference is negligible compared to the speed gain.
    const int compressed_size = LZ4_compress_fast(
        current_input_ptr, 
        compressed_chunk.data(), 
        static_cast<int>(current_chunk_size), 
        max_compressed_size, 
        1 // Acceleration factor
    );

    if (compressed_size <= 0) {
      return -1;
    }

    compressed_chunk.resize(static_cast<std::size_t>(compressed_size));
    total_compressed_size += compressed_size;
    chunks.push_back(std::move(compressed_chunk));
  }

  return total_compressed_size;
}
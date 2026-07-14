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

  const std::size_t input_size = input.size();
  const char* input_ptr = input.data();
  
  // Pre-calculate number of chunks to avoid vector reallocations
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total_compressed_size = 0;

  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t current_chunk_size = std::min(kChunkSize, input_size - offset);
    const char* current_input_ptr = input_ptr + offset;

    // Use a temporary buffer for compression to avoid multiple string resizes
    // LZ4_compressBound provides the worst-case size
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(current_chunk_size));
    
    // We use a local buffer to perform compression, then move it into the vector.
    // To be even faster, we can construct the string with the max size and resize it.
    std::string compressed_chunk;
    compressed_chunk.resize(max_compressed_size);

    const int compressed_size = LZ4_compress_default(
        current_input_ptr,
        compressed_chunk.data(),
        static_cast<int>(current_chunk_size),
        max_compressed_size);

    if (compressed_size <= 0) {
      return -1;
    }

    compressed_chunk.resize(static_cast<std::size_t>(compressed_size));
    total_compressed_size += compressed_size;
    chunks.push_back(std::move(compressed_chunk));
  }

  return total_compressed_size;
}
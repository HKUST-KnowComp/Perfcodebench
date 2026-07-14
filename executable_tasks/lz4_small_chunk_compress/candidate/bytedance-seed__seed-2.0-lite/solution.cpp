#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t input_size = input.size();
  if (input_size == 0) return 0;

  // Pre-reserve space for all chunks to avoid vector reallocations
  const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Allocate a single reusable scratch buffer once, sized for the maximum possible chunk size
  const int max_src_size = static_cast<int>(kChunkSize);
  const std::size_t scratch_size = LZ4_compressBound(max_src_size);
  std::string scratch(scratch_size, '\0');

  int total = 0;
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input_size - offset);
    const int src_size = static_cast<int>(take);
    const char* src = input.data() + offset;

    // Compress directly into the reusable scratch buffer, no source data copy needed
    const int n = LZ4_compress_default(
        src, scratch.data(), src_size, static_cast<int>(scratch_size));
    if (n <= 0) {
      return -1;
    }

    // Construct compressed string in-place in the output vector, only copy actual compressed bytes
    chunks.emplace_back(scratch.data(), n);
    total += n;
  }
  return total;
}
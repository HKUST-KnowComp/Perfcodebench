#include "interface.h"

#include "lz4hc.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  // Pre-calculate number of chunks to reserve vector capacity upfront
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  // Single reusable scratch buffer sized for maximum possible chunk compression output
  const std::size_t max_scratch_size = LZ4_compressBound(static_cast<int>(kChunkSize));
  std::string scratch(max_scratch_size, '\0');
  int total = 0;

  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int n = LZ4_compress_HC(
        input.data() + offset, scratch.data(), static_cast<int>(take), 
        static_cast<int>(max_scratch_size), 12);
    if (n <= 0) {
      return -1;
    }
    // Construct chunk directly in reserved vector memory, only copy actual compressed bytes
    chunks.emplace_back(scratch.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
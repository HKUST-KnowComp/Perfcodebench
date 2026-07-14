#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <vector>
#include <string>

namespace {
// Use larger chunks to reduce per-chunk overhead and improve throughput.
constexpr std::size_t kChunkSize = 1u << 20; // 1 MiB
// Higher acceleration favors speed over ratio, preserving correctness.
constexpr int kAcceleration = 8;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  // Pre-reserve number of chunks to minimize reallocations.
  const std::size_t totalSize = input.size();
  const std::size_t numChunks = (totalSize + kChunkSize - 1) / kChunkSize;
  chunks.reserve(numChunks);

  // Allocate reusable external state for LZ4 compression to avoid per-call setup cost.
  std::vector<char> state(static_cast<std::size_t>(LZ4_sizeofState()));

  int totalCompressed = 0;

  const char* base = input.data();
  const int boundFull = LZ4_compressBound(static_cast<int>(kChunkSize));

  for (std::size_t offset = 0; offset < totalSize; offset += kChunkSize) {
    const std::size_t takeSz = std::min(kChunkSize, totalSize - offset);
    const int take = static_cast<int>(takeSz);
    const int bound = (takeSz == kChunkSize) ? boundFull : LZ4_compressBound(take);

    std::string compressed;
    compressed.resize(static_cast<std::size_t>(bound));

    int n = LZ4_compress_fast_extState(
        state.data(),
        base + offset,
        &compressed[0],
        take,
        bound,
        kAcceleration);

    if (n <= 0) {
      // Compression failed; signal error
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    totalCompressed += n;
    chunks.push_back(std::move(compressed));
  }

  return totalCompressed;
}

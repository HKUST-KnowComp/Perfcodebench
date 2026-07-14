#include "interface.h"

#include "lz4.h"

#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 16; // 64 KiB
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) {
    return 0;
  }

  // Reserve space to avoid vector reallocation overhead
  const std::size_t nChunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(nChunks);

  // Allocate reusable compression state for fast compressor
  const int stateSize = LZ4_sizeofState();
  std::vector<char> state(static_cast<std::size_t>(stateSize));

  const char* src = input.data();
  std::size_t offset = 0;
  std::size_t remaining = input.size();
  int total = 0;

  // Choose an acceleration factor favoring throughput
  const int acceleration = 7;

  // Precompute bound for full-size chunks
  const int fullChunkSize = static_cast<int>(kChunkSize);
  const int fullChunkBound = LZ4_compressBound(fullChunkSize);

  while (remaining > 0) {
    const std::size_t take = remaining >= kChunkSize ? kChunkSize : remaining;
    const int srcSize = static_cast<int>(take);
    const int maxDst = (srcSize == fullChunkSize) ? fullChunkBound : LZ4_compressBound(srcSize);

    chunks.emplace_back();
    std::string& out = chunks.back();
    out.resize(static_cast<std::size_t>(maxDst));

    const int n = LZ4_compress_fast_extState(
        state.data(), src + offset, out.data(), srcSize, maxDst, acceleration);
    if (n <= 0) {
      chunks.pop_back();
      return -1;
    }

    out.resize(static_cast<std::size_t>(n));
    total += n;

    offset += take;
    remaining -= take;
  }

  return total;
}

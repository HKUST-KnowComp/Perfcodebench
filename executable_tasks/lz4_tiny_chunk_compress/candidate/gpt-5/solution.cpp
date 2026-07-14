#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 12; // 4 KB
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t nChunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(nChunks);

  // Reuse a single compression state across chunks for performance.
  std::vector<char> state(LZ4_sizeofState());

  // Precompute maximum destination capacity for typical chunk size.
  const int maxDstCap = LZ4_compressBound(static_cast<int>(kChunkSize));

  int total = 0;
  const char* base = input.data();
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int srcSize = static_cast<int>(take);

    // Allocate output string with enough capacity for worst-case compressed size.
    // Use maxDstCap for all chunks; it's >= required bound for any smaller final chunk.
    std::string compressed;
    compressed.resize(maxDstCap);

    const char* src = base + offset;
    // Choose a moderate acceleration for better speed on tiny chunks.
    // Higher values increase speed at the expense of ratio; correctness is unchanged.
    constexpr int kAcceleration = 12;
    int n = LZ4_compress_fast_extState(state.data(), src, compressed.data(), srcSize, static_cast<int>(compressed.size()), kAcceleration);
    if (n <= 0) {
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    chunks.push_back(std::move(compressed));
  }
  return total;
}

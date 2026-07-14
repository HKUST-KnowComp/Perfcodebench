#include "interface.h"

#include "lz4.h"

#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 16; // 64KB
constexpr int kAcceleration = 4;            // speed/ratio tradeoff; lossless
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) {
    return 0;
  }

  const std::size_t nChunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(nChunks);

  const int scratchCapacity = LZ4_compressBound(static_cast<int>(kChunkSize));
  std::vector<char> scratch(static_cast<std::size_t>(scratchCapacity));

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    std::size_t take = input.size() - offset;
    if (take > kChunkSize) take = kChunkSize;
    const int takeInt = static_cast<int>(take);

    const int n = LZ4_compress_fast(
        input.data() + offset,
        scratch.data(),
        takeInt,
        scratchCapacity,
        kAcceleration);

    if (n <= 0) {
      return -1;
    }

    chunks.emplace_back(scratch.data(), static_cast<std::size_t>(n));
    total += n;
  }

  return total;
}

#include "interface.h"

#include "lz4.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 14; // 16 KiB
constexpr int kAcceleration = 6;            // higher = faster, still lossless
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t inSize = input.size();
  if (inSize == 0) return 0;

  const std::size_t numChunks = (inSize + kChunkSize - 1) / kChunkSize;
  chunks.reserve(numChunks);

  const int stateSize = LZ4_sizeofState();
  if (stateSize <= 0) return -1;
  // Reuse a single compression state for all independent blocks
  std::vector<char> state(static_cast<std::size_t>(stateSize));

  int total = 0;
  std::size_t offset = 0;
  while (offset < inSize) {
    std::size_t remaining = inSize - offset;
    int take = static_cast<int>(remaining < kChunkSize ? remaining : kChunkSize);

    int maxOut = LZ4_compressBound(take);
    chunks.emplace_back(static_cast<std::size_t>(maxOut), '\0');
    std::string& compressed = chunks.back();

    const char* src = input.data() + offset;
    char* dst = &compressed[0];

    int n = LZ4_compress_fast_extState(state.data(), src, dst, take, maxOut, kAcceleration);
    if (n <= 0) {
      return -1;
    }

    compressed.resize(static_cast<std::size_t>(n));
    total += n;
    offset += static_cast<std::size_t>(take);
  }

  return total;
}

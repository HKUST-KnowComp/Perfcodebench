#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  chunks.reserve((input.size() + kChunkSize - 1) / kChunkSize);
  int total = 0;
  LZ4_stream_t state;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(LZ4_compressBound(static_cast<int>(take)));
    LZ4_resetStream(&state);
    const int n = LZ4_compress_fast_extState(
        &state, input.data() + offset, compressed.data(), static_cast<int>(take), static_cast<int>(compressed.size()), 0);
    if (n <= 0) {
      return -1;
    }
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
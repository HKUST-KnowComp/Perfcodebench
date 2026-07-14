#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;   // 64 KiB
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t nChunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(nChunks);

  std::string buffer;                       // reused for every chunk
  buffer.reserve(LZ4_compressBound(static_cast<int>(kChunkSize)));

  int total = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int bound = LZ4_compressBound(static_cast<int>(take));
    buffer.resize(static_cast<std::size_t>(bound));

    const int n = LZ4_compress_fast(
        input.data() + offset,
        buffer.data(),
        static_cast<int>(take),
        bound,
        1);                                 // acceleration=1 gives best speed
    if (n <= 0) return -1;

    chunks.emplace_back(buffer.data(), static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}
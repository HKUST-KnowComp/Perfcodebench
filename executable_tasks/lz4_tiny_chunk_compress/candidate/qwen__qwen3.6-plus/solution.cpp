#include "interface.h"
#include "lz4hc.h"
#include <algorithm>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;

  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);

  int total = 0;
  const int max_compressed = LZ4_compressBound(static_cast<int>(kChunkSize));
  std::string compressed;
  compressed.resize(static_cast<std::size_t>(max_compressed));

  const char* src = input.data();
  char* dst = compressed.data();

  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const int n = LZ4_compress_HC(src + offset, dst, static_cast<int>(take), max_compressed, 12);
    if (n <= 0) return -1;
    chunks.emplace_back(dst, static_cast<std::size_t>(n));
    total += n;
  }
  return total;
}

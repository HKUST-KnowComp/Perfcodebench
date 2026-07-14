#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  if (input.empty()) return 0;
  
  const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
  chunks.reserve(num_chunks);
  
  // Reusable buffer to avoid per-chunk allocation of the worst-case size
  std::vector<char> tmp_buf(LZ4_compressBound(static_cast<int>(kChunkSize)));
  
  int total = 0;
  const char* src = input.data();
  std::size_t remaining = input.size();
  
  while (remaining > 0) {
    const std::size_t take = std::min(kChunkSize, remaining);
    const int n = LZ4_compress_default(
        src, tmp_buf.data(), static_cast<int>(take), static_cast<int>(tmp_buf.size()));
    if (n <= 0) return -1;
    
    chunks.emplace_back(tmp_buf.data(), static_cast<std::size_t>(n));
    total += n;
    src += take;
    remaining -= take;
  }
  return total;
}

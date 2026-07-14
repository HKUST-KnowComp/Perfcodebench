#include "interface.h"
#include "lz4.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  const std::size_t total_len = input.size();
  if (total_len == 0) return 0;
  
  const std::size_t n_chunks = (total_len + kChunkSize - 1) / kChunkSize;
  chunks.reserve(n_chunks);
  
  int total_compressed = 0;
  const char* src = input.data();
  std::size_t remaining = total_len;
  
  for (std::size_t i = 0; i < n_chunks; ++i) {
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    
    chunks.emplace_back();
    std::string& out = chunks.back();
    out.resize(LZ4_compressBound(static_cast<int>(take)));
    
    const int n = LZ4_compress_default(
        src, out.data(), static_cast<int>(take), static_cast<int>(out.size()));
    if (n <= 0) return -1;
    
    out.resize(static_cast<std::size_t>(n));
    total_compressed += n;
    src += take;
    remaining -= take;
  }
  
  return total_compressed;
}
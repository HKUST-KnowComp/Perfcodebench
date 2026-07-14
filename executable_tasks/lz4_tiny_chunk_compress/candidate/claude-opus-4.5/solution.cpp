#include "interface.h"

#include "lz4.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
  chunks.clear();
  
  if (input.empty()) {
    return 0;
  }
  
  const std::size_t inputSize = input.size();
  const std::size_t numChunks = (inputSize + kChunkSize - 1) / kChunkSize;
  chunks.reserve(numChunks);
  
  const int bound = LZ4_compressBound(static_cast<int>(kChunkSize));
  
  int total = 0;
  const char* src = input.data();
  
  for (std::size_t offset = 0; offset < inputSize; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, inputSize - offset);
    const int srcSize = static_cast<int>(take);
    
    chunks.emplace_back();
    std::string& compressed = chunks.back();
    compressed.resize(static_cast<std::size_t>(bound));
    
    const int n = LZ4_compress_default(
        src + offset,
        &compressed[0],
        srcSize,
        bound);
    
    if (n <= 0) {
      return -1;
    }
    
    compressed.resize(static_cast<std::size_t>(n));
    total += n;
  }
  
  return total;
}
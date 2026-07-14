#include "interface.h"

#include "lz4.h"

#include <cstddef>
#include <cstring>

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
  
  const int maxCompressedSize = LZ4_compressBound(static_cast<int>(kChunkSize));
  std::string compressBuffer;
  compressBuffer.resize(static_cast<std::size_t>(maxCompressedSize));
  
  const char* src = input.data();
  int total = 0;
  
  for (std::size_t offset = 0; offset < inputSize; offset += kChunkSize) {
    const std::size_t take = (offset + kChunkSize <= inputSize) ? kChunkSize : (inputSize - offset);
    
    const int n = LZ4_compress_default(
        src + offset,
        compressBuffer.data(),
        static_cast<int>(take),
        maxCompressedSize);
    
    if (n <= 0) {
      return -1;
    }
    
    chunks.emplace_back(compressBuffer.data(), static_cast<std::size_t>(n));
    total += n;
  }
  
  return total;
}
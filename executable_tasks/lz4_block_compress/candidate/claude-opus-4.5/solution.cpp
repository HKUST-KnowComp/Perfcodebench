#include "interface.h"

#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
  const int inputSize = static_cast<int>(input.size());
  const int maxCompressedSize = LZ4_compressBound(inputSize);
  compressed.resize(static_cast<std::size_t>(maxCompressedSize));
  const int n = LZ4_compress_default(
      input.data(), compressed.data(), inputSize, maxCompressedSize);
  if (n <= 0) {
    return -1;
  }
  compressed.resize(static_cast<std::size_t>(n));
  return n;
}

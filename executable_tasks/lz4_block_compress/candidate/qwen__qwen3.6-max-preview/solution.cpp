#include "interface.h"
#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
  const int srcSize = static_cast<int>(input.size());
  compressed.resize(LZ4_compressBound(srcSize));
  const int n = LZ4_compress_default(input.data(), compressed.data(), srcSize, static_cast<int>(compressed.size()));
  if (n <= 0) {
    return -1;
  }
  compressed.resize(static_cast<std::size_t>(n));
  return n;
}
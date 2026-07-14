#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  const size_t max_size = ZSTD_compressBound(input.size());
  compressed.resize(max_size);
  const size_t n = ZSTD_compress(
      compressed.data(), max_size, input.data(), input.size(), 19);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}
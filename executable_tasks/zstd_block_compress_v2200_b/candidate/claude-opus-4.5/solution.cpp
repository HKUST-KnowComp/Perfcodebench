#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  size_t n = ZSTD_compress(
      const_cast<char*>(compressed.data()), bound,
      input.data(), input.size(), 1);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}
#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  compressed.resize(ZSTD_compressBound(input.size()));
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), input.data(), input.size(), 3);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}

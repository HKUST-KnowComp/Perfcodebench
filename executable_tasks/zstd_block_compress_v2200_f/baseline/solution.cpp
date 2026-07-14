#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  const std::string local = input;
  compressed.resize(ZSTD_compressBound(local.size()));
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), local.data(), local.size(), 19);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}

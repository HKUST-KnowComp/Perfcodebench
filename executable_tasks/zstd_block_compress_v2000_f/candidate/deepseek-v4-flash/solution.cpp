#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  size_t const cBuffSize = ZSTD_compressBound(input.size());
  compressed.resize(cBuffSize);
  size_t const cSize = ZSTD_compress(
      compressed.data(), compressed.size(),
      input.data(), input.size(),
      1);
  if (ZSTD_isError(cSize)) {
    return -1;
  }
  compressed.resize(cSize);
  return static_cast<int>(cSize);
}

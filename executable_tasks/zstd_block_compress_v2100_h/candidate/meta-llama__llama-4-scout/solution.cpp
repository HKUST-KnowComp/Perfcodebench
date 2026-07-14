#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  const size_t input_size = input.size();
  const size_t max_compressed_size = ZSTD_compressBound(input_size);
  compressed.resize(max_compressed_size);
  const size_t n = ZSTD_compress(
      compressed.data(), max_compressed_size, input.data(), input_size, 22);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}
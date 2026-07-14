#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Avoid copying the input string (removed 'const std::string local = input;')
  // Use ZSTD_compressBound to allocate sufficient space once
  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  // Use compression level 3 for a better balance of speed and ratio compared to 19
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input.data(), input.size(), 
      3);

  if (ZSTD_isError(n)) {
    return -1;
  }

  // Shrink the string to the actual compressed size
  compressed.resize(n);
  return static_cast<int>(n);
}
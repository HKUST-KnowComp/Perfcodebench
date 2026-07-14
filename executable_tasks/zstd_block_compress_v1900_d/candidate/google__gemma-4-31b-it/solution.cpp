#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Avoid copying the input string (removed 'const std::string local = input;')
  // Use ZSTD_compressBound to allocate the maximum possible size needed for the compressed output
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  // Compress directly from the input string's buffer
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input.data(), input.size(), 
      19);

  if (ZSTD_isError(n)) {
    return -1;
  }

  // Shrink the output string to the actual compressed size
  compressed.resize(n);
  return static_cast<int>(n);
}
#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Remove the local copy of the input string to avoid unnecessary allocation and memcpy
  const size_t input_size = input.size();
  const void* input_ptr = input.data();

  // Pre-allocate the output buffer based on ZSTD's upper bound
  compressed.resize(ZSTD_compressBound(input_size));

  // Use compression level 3 instead of 19. 
  // Level 19 is extremely slow and intended for archival, whereas level 3 is the 
  // balanced default providing high throughput and good compression.
  const size_t compressed_size = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input_ptr, input_size, 
      3);

  if (ZSTD_isError(compressed_size)) {
    return -1;
  }

  // Shrink the string to the actual size of the compressed data
  compressed.resize(compressed_size);
  return static_cast<int>(compressed_size);
}
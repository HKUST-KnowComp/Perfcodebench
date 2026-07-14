#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Avoid copying the input string
  const size_t input_size = input.size();
  const void* input_ptr = input.data();

  // Pre-allocate the buffer to the maximum possible size to avoid reallocations
  const size_t bound = ZSTD_compressBound(input_size);
  compressed.resize(bound);

  // Use compression level 3 for a balance of speed and ratio. 
  // Level 19 is extremely slow and usually overkill for general block compression.
  const size_t compressed_size = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input_ptr, input_size, 
      3);

  if (ZSTD_isError(compressed_size)) {
    return -1;
  }

  // Shrink the string to the actual compressed size
  compressed.resize(compressed_size);
  return static_cast<int>(compressed_size);
}
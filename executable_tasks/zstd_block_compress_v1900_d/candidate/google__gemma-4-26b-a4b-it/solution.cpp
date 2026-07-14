#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  if (input.empty()) {
    compressed.clear();
    return 0;
  }

  size_t const input_size = input.size();
  size_t const max_compressed_size = ZSTD_compressBound(input_size);

  // Pre-allocate/resize the output buffer once to avoid multiple reallocations.
  // We use resize() to ensure the buffer is large enough for ZSTD_compress.
  compressed.resize(max_compressed_size);

  // Perform compression directly using the input string's data pointer.
  // Level 19 is kept as per the original requirement to preserve compression ratio/behavior.
  size_t const compressed_size = ZSTD_compress(
      compressed.data(), 
      max_compressed_size, 
      input.data(), 
      input_size, 
      19);

  if (ZSTD_isError(compressed_size)) {
    return -1;
  }

  // Shrink the string to the actual compressed size.
  compressed.resize(compressed_size);
  return static_cast<int>(compressed_size);
}
#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  size_t input_size = input.size();
  size_t max_compressed_size = ZSTD_compressBound(input_size);
  
  // Pre-allocate/resize the output buffer once to avoid multiple reallocations
  compressed.resize(max_compressed_size);

  // Using a lower compression level (e.g., 3) significantly improves throughput.
  // Level 19 is extremely slow and usually not suitable for high-throughput tasks.
  size_t const c_size = ZSTD_compress(
      compressed.data(), 
      max_compressed_size, 
      input.data(), 
      input_size, 
      3);

  if (ZSTD_isError(c_size)) {
    return -1;
  }

  // Shrink the string to the actual compressed size
  compressed.resize(c_size);
  return static_cast<int>(c_size);
}
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
  
  // Pre-allocate the buffer to avoid multiple reallocations
  compressed.resize(max_compressed_size);

  // Using a lower compression level (e.g., 3) significantly improves throughput
  // while maintaining high compression ratios for most data types.
  // Level 19 is too slow for performance-critical throughput tasks.
  size_t const c_size = ZSTD_compress(
      compressed.data(), 
      max_compressed_size, 
      input.data(), 
      input_size, 
      3);

  if (ZSTD_isError(c_size)) {
    return -1;
  }

  compressed.resize(c_size);
  return static_cast<int>(c_size);
}
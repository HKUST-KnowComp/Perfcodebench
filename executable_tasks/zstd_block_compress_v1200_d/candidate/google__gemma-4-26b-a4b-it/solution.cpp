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
  
  // Pre-allocate memory to avoid multiple reallocations
  compressed.resize(max_compressed_size);

  // Using compression level 3 for significantly higher throughput on large blocks.
  // Level 19 is extremely slow and usually unnecessary for throughput-oriented tasks.
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
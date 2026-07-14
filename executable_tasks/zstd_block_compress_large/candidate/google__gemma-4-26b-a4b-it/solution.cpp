#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  size_t input_size = input.size();
  size_t max_compressed_size = ZSTD_compressBound(input_size);
  
  // Pre-allocate memory to avoid multiple reallocations
  compressed.resize(max_compressed_size);

  // Use a lower compression level (e.g., 3) to significantly increase throughput.
  // Level 19 is extremely slow and usually unnecessary for general throughput tasks.
  // We also avoid the 'const std::string local = input;' copy which was doubling memory usage.
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
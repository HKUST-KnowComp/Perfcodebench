#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Remove the local copy of input to avoid unnecessary allocation and memory bandwidth usage
  size_t const input_size = input.size();
  size_t const bound = ZSTD_compressBound(input_size);
  
  compressed.resize(bound);
  
  // Compression level 19 is extremely slow. Level 3 is the Zstd default and provides 
  // a much better balance of speed and ratio for performance benchmarks.
  size_t const n = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input.data(), input_size, 
      3);

  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  
  // Using compression level 3 is the Zstandard default and provides the best 
  // balance between speed and ratio for general-purpose block compression.
  size_t n = ZSTD_compress(
      &compressed[0], compressed.size(), 
      input.data(), input.size(), 
      3);
      
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
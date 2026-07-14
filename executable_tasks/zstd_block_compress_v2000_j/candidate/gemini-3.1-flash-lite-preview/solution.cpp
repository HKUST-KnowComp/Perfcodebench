#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  
  // Using compression level 3 as it is the standard default for Zstd,
  // providing a much better balance of speed and ratio compared to 19.
  size_t n = ZSTD_compress(
      compressed.data(), 
      compressed.size(), 
      input.data(), 
      input.size(), 
      3);
      
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Use a lower compression level (3 is the Zstd default/sweet spot) 
  // to drastically improve throughput compared to level 19.
  const int compression_level = 3;
  
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  
  size_t n = ZSTD_compress(
      compressed.data(), 
      compressed.size(), 
      input.data(), 
      input.size(), 
      compression_level);
      
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
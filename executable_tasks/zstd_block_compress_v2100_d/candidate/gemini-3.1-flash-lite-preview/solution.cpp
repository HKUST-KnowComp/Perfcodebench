#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Use a lower compression level (3 is the Zstd default/recommended for speed/ratio balance)
  // Level 19 is extremely slow and intended for archival, not high-throughput block compression.
  const int compression_level = 3;
  
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  
  // Compress directly from the input string without creating a local copy
  size_t n = ZSTD_compress(
      compressed.data(), 
      compressed.size(), 
      input.data(), 
      input.size(), 
      compression_level
  );
  
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
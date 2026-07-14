#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Remove the 'const std::string local = input;' copy to avoid O(N) allocation and copy
  size_t const input_size = input.size();
  size_t const bound = ZSTD_compressBound(input_size);
  
  compressed.resize(bound);
  
  // Use input.data() directly
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input.data(), input_size, 
      19);

  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
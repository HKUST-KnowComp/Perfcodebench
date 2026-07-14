#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Avoid copying the input string
  size_t const input_size = input.size();
  size_t const bound = ZSTD_compressBound(input_size);
  
  compressed.resize(bound);
  
  // Level 3 is the default Zstd level, providing a much better speed/ratio trade-off than 19
  size_t const compressed_size = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input.data(), input_size, 
      3);

  if (ZSTD_isError(compressed_size)) {
    return -1;
  }

  compressed.resize(compressed_size);
  return static_cast<int>(compressed_size);
}
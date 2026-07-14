#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Avoid copying the input string; use the reference directly.
  size_t const input_size = input.size();
  size_t const bound = ZSTD_compressBound(input_size);
  
  compressed.resize(bound);
  
  // Compression level 3 is the Zstd default and offers a much better 
  // performance/compression ratio than level 19 for general throughput.
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
#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Avoid copying the input string; use the reference directly.
  const size_t input_size = input.size();
  const size_t bound = ZSTD_compressBound(input_size);
  
  compressed.resize(bound);
  
  // Compression level 3 is the Zstd default and offers a much better 
  // performance/compression ratio than level 19 for general throughput.
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), 
      input.data(), input_size, 3);

  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
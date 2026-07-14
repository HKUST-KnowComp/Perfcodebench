#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Use ZSTD_compressBound to pre-allocate the worst-case buffer size
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  // Use compression level 3 for optimal throughput/ratio balance.
  // Avoided unnecessary string copy by using input.data() directly.
  size_t n = ZSTD_compress(
      compressed.data(), 
      compressed.size(), 
      input.data(), 
      input.size(), 
      3
  );

  if (ZSTD_isError(n)) {
    return -1;
  }

  // Shrink the buffer to the actual compressed size
  compressed.resize(n);
  return static_cast<int>(n);
}
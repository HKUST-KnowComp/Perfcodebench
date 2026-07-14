#include "interface.h"
#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
  if (input.empty()) {
    compressed.clear();
    return 0;
  }

  const int input_size = static_cast<int>(input.size());
  const int max_compressed_size = LZ4_compressBound(input_size);
  
  // Pre-allocate the buffer to avoid multiple reallocations
  compressed.resize(max_compressed_size);

  // Use the high-speed default compression instead of HC (High Compression)
  // to significantly increase throughput (bytes/sec).
  const int compressed_size = LZ4_compress_default(
      input.data(), 
      compressed.data(), 
      input_size, 
      max_compressed_size
  );

  if (compressed_size <= 0) {
    return -1;
  }

  // Shrink to the actual compressed size
  compressed.resize(static_cast<std::size_t>(compressed_size));
  return compressed_size;
}
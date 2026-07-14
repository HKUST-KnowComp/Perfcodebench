#include "interface.h"
#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
  if (input.empty()) {
    compressed.clear();
    return 0;
  }
  
  int max_size = LZ4_compressBound(static_cast<int>(input.size()));
  compressed.resize(max_size);
  
  int n = LZ4_compress_default(
      input.data(), 
      compressed.data(), 
      static_cast<int>(input.size()), 
      max_size
  );
  
  if (n < 0) {
    return -1;
  }
  
  compressed.resize(static_cast<std::size_t>(n));
  return n;
}
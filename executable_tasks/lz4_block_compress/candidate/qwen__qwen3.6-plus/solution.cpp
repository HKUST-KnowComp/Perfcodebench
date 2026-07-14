#include "interface.h"
#include "lz4.h"

int compress_payload(const std::string& input, std::string& compressed) {
  const int src_size = static_cast<int>(input.size());
  const int max_dst_size = LZ4_compressBound(src_size);
  compressed.resize(max_dst_size);
  const int n = LZ4_compress_default(input.data(), compressed.data(), src_size, max_dst_size);
  if (n < 0) {
    return -1;
  }
  compressed.resize(static_cast<std::size_t>(n));
  return n;
}

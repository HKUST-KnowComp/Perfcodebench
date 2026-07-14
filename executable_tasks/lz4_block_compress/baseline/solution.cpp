#include "interface.h"

#include "lz4hc.h"

int compress_payload(const std::string& input, std::string& compressed) {
  compressed.resize(LZ4_compressBound(static_cast<int>(input.size())));
  const int n = LZ4_compress_HC(
      input.data(), compressed.data(), static_cast<int>(input.size()), static_cast<int>(compressed.size()), 12);
  if (n < 0) {
    return -1;
  }
  compressed.resize(static_cast<std::size_t>(n));
  return n;
}

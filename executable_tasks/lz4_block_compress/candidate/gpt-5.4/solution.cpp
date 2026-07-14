#include "interface.h"

#include "lz4.h"
#include "lz4hc.h"

int compress_payload(const std::string& input, std::string& compressed) {
  const int input_size = static_cast<int>(input.size());
  const int bound = LZ4_compressBound(input_size);

  compressed.resize(static_cast<std::size_t>(bound));

  // Use the default fast compressor for maximum throughput.
  // It preserves lossless roundtrip correctness while being much faster
  // than high-compression mode for large buffers.
  const int n = LZ4_compress_default(
      input.data(), compressed.data(), input_size, bound);

  if (n <= 0) {
    compressed.clear();
    return -1;
  }

  compressed.resize(static_cast<std::size_t>(n));
  return n;
}

#include "interface.h"

#include "lz4.h"
#include <limits>

int compress_payload(const std::string& input, std::string& compressed) {
  // Guard against overly large inputs that exceed LZ4 API int size limits
  if (input.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    return -1;
  }

  const int srcSize = static_cast<int>(input.size());
  if (srcSize == 0) {
    compressed.clear();
    return 0;
  }

  const int maxDst = LZ4_compressBound(srcSize);
  compressed.resize(static_cast<std::size_t>(maxDst));

  // Higher acceleration => faster, lower ratio. Choose a value favoring throughput.
  constexpr int kAcceleration = 16;
  const int n = LZ4_compress_fast(input.data(), compressed.data(), srcSize, maxDst, kAcceleration);
  if (n <= 0) {
    return -1;
  }

  compressed.resize(static_cast<std::size_t>(n));
  return n;
}

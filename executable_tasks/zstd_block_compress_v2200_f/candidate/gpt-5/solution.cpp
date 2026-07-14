#include "interface.h"
#include "zstd.h"

#include <string>
#include <limits>

int compress_payload(const std::string& input, std::string& compressed) {
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    if (!cctx) {
      return -1;
    }
  }

  const size_t srcSize = input.size();
  const size_t dstCapacity = ZSTD_compressBound(srcSize);
  compressed.resize(dstCapacity);

  // Use a fast compression level for higher throughput.
  const int level = 1;
  size_t written = ZSTD_compressCCtx(
      cctx, compressed.data(), dstCapacity, input.data(), srcSize, level);

  if (ZSTD_isError(written)) {
    return -1;
  }

  compressed.resize(written);
  if (written > static_cast<size_t>(std::numeric_limits<int>::max())) {
    return -1;
  }
  return static_cast<int>(written);
}

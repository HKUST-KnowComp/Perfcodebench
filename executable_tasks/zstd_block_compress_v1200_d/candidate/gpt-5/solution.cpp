#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a thread-local compression context to reduce per-call overhead.
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    if (!cctx) {
      return -1;
    }
  }

  const size_t srcSize = input.size();
  const size_t maxDst = ZSTD_compressBound(srcSize);
  compressed.resize(maxDst);

  // Use a fast compression level for throughput.
  const int level = 1;
  const size_t n = ZSTD_compressCCtx(
      cctx,
      compressed.data(),
      maxDst,
      input.data(),
      srcSize,
      level);

  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}

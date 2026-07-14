#include "interface.h"

#include "zstd.h"

#include <string>
#include <limits>

int compress_payload(const std::string& input, std::string& compressed) {
  // Fast path for empty input
  if (input.empty()) {
    compressed.clear();
    return 0;
  }

  // Reuse a compression context across calls (thread-local for safety)
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    return -1;
  }

  // One-time parameter setup per thread
  static thread_local bool params_set = false;
  if (!params_set) {
    // Favor speed: level 1 is significantly faster than high levels
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    // Write the content size into the frame header to enable potential optimizations
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 1);
    params_set = true;
  }

  const size_t srcSize = input.size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  // Inform the context of the total size (pledged size)
  (void)ZSTD_CCtx_setPledgedSrcSize(cctx, srcSize);

  const size_t n = ZSTD_compress2(cctx, compressed.data(), dstCap, input.data(), srcSize);
  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

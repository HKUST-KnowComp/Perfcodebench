#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context across calls to avoid repeated allocations/init costs
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    if (!cctx) {
      return -1;
    }
    // Favor speed for throughput while preserving correctness
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    // Do not write content size in the frame header (minor speed/size tweak)
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 0);
  }

  const size_t srcSize = input.size();
  const size_t maxDst = ZSTD_compressBound(srcSize);
  compressed.resize(maxDst);

  size_t const n = ZSTD_compress2(
      cctx,
      compressed.data(), compressed.size(),
      input.data(), srcSize);

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context per thread to avoid repeated allocations and setup
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    if (!cctx) {
      return -1;
    }
    // Choose a fast compression level for throughput
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
  } else {
    // Reset session state while keeping parameters for reuse
    (void)ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);
  }

  const size_t srcSize = input.size();
  const size_t bound = ZSTD_compressBound(srcSize);
  compressed.resize(bound);

  const size_t result = ZSTD_compress2(
      cctx,
      compressed.data(), bound,
      input.data(), srcSize);

  if (ZSTD_isError(result)) {
    compressed.clear();
    return -1;
  }

  compressed.resize(result);
  return static_cast<int>(result);
}

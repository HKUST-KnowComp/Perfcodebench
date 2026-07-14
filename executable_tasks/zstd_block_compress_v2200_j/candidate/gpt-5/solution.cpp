#include "interface.h"
#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context across calls for better performance.
  static thread_local ZSTD_CCtx* ctx = nullptr;
  static thread_local bool configured = false;

  if (!ctx) {
    ctx = ZSTD_createCCtx();
    if (!ctx) return -1;
  }
  if (!configured) {
    // Favor throughput: fast level, no checksum, no content size in frame.
    (void)ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 1);
    (void)ZSTD_CCtx_setParameter(ctx, ZSTD_c_checksumFlag, 0);
    (void)ZSTD_CCtx_setParameter(ctx, ZSTD_c_contentSizeFlag, 0);
    configured = true;
  }

  const size_t srcSize = input.size();
  const size_t bound = ZSTD_compressBound(srcSize);
  compressed.resize(bound);

  const size_t n = ZSTD_compress2(
      ctx,
      compressed.data(), bound,
      input.data(), srcSize);

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

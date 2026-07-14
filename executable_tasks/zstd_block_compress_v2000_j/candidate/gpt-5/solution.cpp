#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context to avoid reallocations and setup overhead.
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();

  const size_t srcSize = input.size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  size_t written;
  if (cctx) {
    // Use a fast compression level for high throughput.
    written = ZSTD_compressCCtx(
        cctx,
        compressed.data(), dstCap,
        input.data(), srcSize,
        1 /* fast level for throughput */);
  } else {
    // Fallback in the unlikely event context creation failed.
    written = ZSTD_compress(
        compressed.data(), dstCap,
        input.data(), srcSize,
        1);
  }

  if (ZSTD_isError(written)) {
    return -1;
  }

  compressed.resize(written);
  return static_cast<int>(written);
}

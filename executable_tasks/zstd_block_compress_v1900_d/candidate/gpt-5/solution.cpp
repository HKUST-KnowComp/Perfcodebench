#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context to avoid setup overhead across calls
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    return -1;
  }

  const size_t srcSize = input.size();
  const size_t maxDst = ZSTD_compressBound(srcSize);

  // Ensure output buffer is large enough
  compressed.resize(maxDst);

  // Use a fast compression level for throughput
  const size_t n = ZSTD_compressCCtx(
      cctx,
      compressed.data(), maxDst,
      input.data(), srcSize,
      1  // compression level tuned for speed
  );

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

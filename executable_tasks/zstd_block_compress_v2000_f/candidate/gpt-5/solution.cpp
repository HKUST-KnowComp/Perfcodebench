#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    if (!cctx) return -1;
    // Avoid storing content size in the frame header to reduce overhead.
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 0);
  }

  const size_t srcSize = input.size();
  const size_t dstCapacity = ZSTD_compressBound(srcSize);
  compressed.resize(dstCapacity);

  const int level = 1; // faster compression level
  const size_t cSize = ZSTD_compressCCtx(
      cctx,
      compressed.data(), dstCapacity,
      input.data(), srcSize,
      level);

  if (ZSTD_isError(cSize)) {
    return -1;
  }
  compressed.resize(cSize);
  return static_cast<int>(cSize);
}

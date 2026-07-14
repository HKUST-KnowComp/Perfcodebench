#include "interface.h"
#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context per thread to minimize setup overhead.
  thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    return -1;
  }

  // Configure once per thread: prioritize speed with a low compression level.
  thread_local bool configured = false;
  if (!configured) {
    size_t rc = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    if (ZSTD_isError(rc)) {
      return -1;
    }
    configured = true;
  }

  const size_t srcSize = input.size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  // Provide the total source size to the compressor for potential optimizations.
  size_t rc = ZSTD_CCtx_setPledgedSrcSize(cctx, srcSize);
  if (ZSTD_isError(rc)) {
    return -1;
  }

  size_t n = ZSTD_compress2(cctx,
                            compressed.data(), dstCap,
                            input.data(), srcSize);
  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

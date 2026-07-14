#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reusable compression context avoids allocation per call (thread-safe with thread_local).
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    return -1;
  }

  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  // Use fast compression level 1 instead of 19 for throughput; correctness preserved.
  const size_t cSize = ZSTD_compressCCtx(cctx,
      compressed.data(), bound,
      input.data(), input.size(),
      1);

  if (ZSTD_isError(cSize)) {
    return -1;
  }

  compressed.resize(cSize);
  return static_cast<int>(cSize);
}
#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Use thread-local context to amortize initialization cost
  thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  
  if (!cctx) {
    return -1;
  }
  
  // Resize output buffer to worst-case bound
  compressed.resize(ZSTD_compressBound(input.size()));
  
  // Compress directly from input (no copy) with level 3 for throughput
  // Level 3 provides good balance: 10-50x faster than level 19
  const size_t n = ZSTD_compressCCtx(
      cctx,
      compressed.data(), compressed.size(),
      input.data(), input.size(),
      3);
  
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
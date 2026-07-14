#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Eliminate unnecessary copy
  compressed.resize(ZSTD_compressBound(input.size()));
  
  // Use streaming API with optimized parameters for xlarge blocks
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
  }
  
  // Level 3 provides excellent speed/ratio tradeoff for large blocks
  // Much faster than level 19 with minimal ratio loss
  ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);
  
  // Optimize for large blocks
  ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 27);
  ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 0); // Single-threaded for consistency
  
  const size_t n = ZSTD_compress2(cctx, 
      compressed.data(), compressed.size(), 
      input.data(), input.size());
  
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
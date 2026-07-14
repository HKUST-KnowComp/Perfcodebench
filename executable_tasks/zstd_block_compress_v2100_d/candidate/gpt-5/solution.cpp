#include "interface.h"

#include "zstd.h"

#include <string>
#include <limits>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a persistent compression context for better performance across calls
  static thread_local ZSTD_CCtx* cctx = nullptr;
  if (cctx == nullptr) {
    cctx = ZSTD_createCCtx();
    if (cctx == nullptr) {
      return -1;
    }
    // Prefer fast compression for throughput
    size_t r = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    if (ZSTD_isError(r)) {
      // Proceed with default parameters if setting fails
    }
  }

  // Enable multithreaded compression for large inputs when available
  // Fallback silently to single-thread if MT is unsupported or for small inputs
  if (input.size() >= (1u << 20)) { // Threshold: 1 MiB
    size_t r = ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 4);
    if (ZSTD_isError(r)) {
      (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 0);
    }
  } else {
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 0);
  }

  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  const size_t n = ZSTD_compress2(cctx, compressed.data(), bound, input.data(), input.size());
  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  if (n > static_cast<size_t>(std::numeric_limits<int>::max())) {
    return -1;
  }
  return static_cast<int>(n);
}

#include "interface.h"

#include "zstd.h"

#include <string>
#include <limits>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context across calls for performance
  static thread_local ZSTD_CCtx* cctx = nullptr;
  static thread_local bool params_set = false;

  if (cctx == nullptr) {
    cctx = ZSTD_createCCtx();
    if (cctx == nullptr) {
      return -1;
    }
    params_set = false;
  }

  // Configure context parameters once (fastest settings that preserve correctness)
  if (!params_set) {
    // Fastest reasonable compression level for throughput
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    // Use fast strategy to favor speed
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, ZSTD_fast);
    // Disable checksum to save a tiny bit of work
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 0);
    // Enable multi-threading if library supports it (non-zero workers)
    (void)ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 2);
    params_set = true;
  }

  const size_t srcSize = input.size();
  const size_t bound = ZSTD_compressBound(srcSize);
  compressed.resize(bound);

  const size_t n = ZSTD_compress2(cctx, compressed.data(), bound, input.data(), srcSize);
  if (ZSTD_isError(n)) {
    return -1;
  }

  if (n > static_cast<size_t>(std::numeric_limits<int>::max())) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

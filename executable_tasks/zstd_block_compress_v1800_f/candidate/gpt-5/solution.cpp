#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a compression context across calls to minimize setup/teardown cost
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    return -1;
  }

  const size_t srcSize = input.size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  // Favor throughput: fastest preset level
  const int level = 1;
  const size_t n = ZSTD_compressCCtx(
      cctx, compressed.data(), dstCap, input.data(), srcSize, level);

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

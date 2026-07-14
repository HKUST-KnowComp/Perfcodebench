#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  const size_t srcSize = input.size();
  const size_t bound = ZSTD_compressBound(srcSize);
  compressed.resize(bound);
  
  static ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) return -1;
  
  size_t n = ZSTD_compressCCtx(cctx, compressed.data(), bound, input.data(), srcSize, 1);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}

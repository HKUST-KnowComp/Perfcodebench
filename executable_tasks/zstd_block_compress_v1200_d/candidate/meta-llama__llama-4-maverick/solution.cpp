#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    return -1;
  }
  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  const size_t n = ZSTD_compressCCtx(cctx, compressed.data(), bound, input.data(), input.size(), 3);
  ZSTD_freeCCtx(cctx);
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}
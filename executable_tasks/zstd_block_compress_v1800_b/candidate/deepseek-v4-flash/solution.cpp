#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  static ZSTD_CCtx* cctx = nullptr;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    if (!cctx) return -1;
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
  }

  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  const size_t n = ZSTD_compress2(cctx,
                                  compressed.data(), compressed.size(),
                                  input.data(), input.size());
  if (ZSTD_isError(n)) {
    return -1;
  }
  compressed.resize(n);
  return static_cast<int>(n);
}

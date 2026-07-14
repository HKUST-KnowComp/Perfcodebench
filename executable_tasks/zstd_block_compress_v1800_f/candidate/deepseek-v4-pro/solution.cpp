#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  static thread_local ZSTD_CCtx* cctx = []{
    ZSTD_CCtx* ctx = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, 1);
    return ctx;
  }();
  compressed.resize(ZSTD_compressBound(input.size()));
  const size_t n = ZSTD_compress2(cctx, compressed.data(), compressed.size(),
                                  input.data(), input.size());
  if (ZSTD_isError(n)) return -1;
  compressed.resize(n);
  return static_cast<int>(n);
}
#include "interface.h"

#include "zstd.h"

#include <string>

namespace {
struct CCtxWrapper {
    ZSTD_CCtx* cctx;
    CCtxWrapper() { cctx = ZSTD_createCCtx(); }
    ~CCtxWrapper() { if (cctx) ZSTD_freeCCtx(cctx); }
};
}

int compress_payload(const std::string& input, std::string& compressed) {
  static thread_local CCtxWrapper ctx;
  if (!ctx.cctx) {
    return -1;
  }
  
  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  
  size_t n = ZSTD_compressCCtx(
      ctx.cctx, &compressed[0], bound, input.data(), input.size(), 1);
      
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}

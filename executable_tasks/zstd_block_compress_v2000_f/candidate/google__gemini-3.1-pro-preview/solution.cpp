#include "interface.h"
#include "zstd.h"
#include <string>
#include <memory>

struct CCtxDeleter {
  void operator()(ZSTD_CCtx* ctx) const {
    ZSTD_freeCCtx(ctx);
  }
};

int compress_payload(const std::string& input, std::string& compressed) {
  thread_local std::unique_ptr<ZSTD_CCtx, CCtxDeleter> cctx(ZSTD_createCCtx());
  if (!cctx) return -1;

  size_t bound = ZSTD_compressBound(input.size());
  
  thread_local std::unique_ptr<char[]> buffer;
  thread_local size_t buffer_size = 0;
  
  if (buffer_size < bound) {
    buffer.reset(new char[bound]);
    buffer_size = bound;
  }

  const size_t n = ZSTD_compressCCtx(
      cctx.get(), buffer.get(), buffer_size, input.data(), input.size(), 1);
      
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.assign(buffer.get(), n);
  return static_cast<int>(n);
}

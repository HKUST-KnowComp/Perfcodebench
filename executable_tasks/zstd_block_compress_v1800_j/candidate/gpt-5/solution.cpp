#include "interface.h"

#include "zstd.h"

#include <string>

namespace {
struct CtxHolder {
  ZSTD_CCtx* ctx;
  CtxHolder() : ctx(ZSTD_createCCtx()) {}
  ~CtxHolder() {
    if (ctx) ZSTD_freeCCtx(ctx);
  }
};
}

int compress_payload(const std::string& input, std::string& compressed) {
  const size_t srcSize = input.size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  // Reuse a thread-local compression context to reduce per-call overhead.
  thread_local CtxHolder holder;
  ZSTD_CCtx* ctx = holder.ctx;
  if (!ctx) {
    // Fallback in the unlikely event the context wasn't created.
    holder.ctx = ZSTD_createCCtx();
    ctx = holder.ctx;
    if (!ctx) return -1;
  }

  // Use a fast compression level for higher throughput while preserving correctness.
  const int compressionLevel = 1;
  const size_t n = ZSTD_compressCCtx(
      ctx, compressed.data(), compressed.size(), input.data(), srcSize, compressionLevel);
  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

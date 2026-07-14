#include "interface.h"
#include "zstd.h"

#include <string>

namespace {
struct ThreadLocalCCtx {
  ZSTD_CCtx* ctx;
  ThreadLocalCCtx() : ctx(ZSTD_createCCtx()) {}
  ~ThreadLocalCCtx() { if (ctx) ZSTD_freeCCtx(ctx); }
};
}

int compress_payload(const std::string& input, std::string& compressed) {
  // Reuse a thread-local compression context to reduce setup overhead.
  static thread_local ThreadLocalCCtx holder;
  ZSTD_CCtx* ctx = holder.ctx;

  const size_t srcSize = input.size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  // Use a faster compression level for throughput.
  const int level = 1;

  size_t n;
  if (ctx) {
    n = ZSTD_compressCCtx(ctx, compressed.data(), dstCap, input.data(), srcSize, level);
  } else {
    // Fallback in unlikely case context creation failed.
    n = ZSTD_compress(compressed.data(), dstCap, input.data(), srcSize, level);
  }

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}
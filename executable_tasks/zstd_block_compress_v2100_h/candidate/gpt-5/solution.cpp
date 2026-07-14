#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Handle potential aliasing: if input and compressed refer to the same object,
  // make a temporary copy to avoid corrupting the source during output resize.
  const std::string* srcPtr = &input;
  std::string temp;
  if (reinterpret_cast<const void*>(&input) == reinterpret_cast<const void*>(&compressed)) {
    temp = input;
    srcPtr = &temp;
  }

  // Create and reuse a thread-local ZSTD compression context to avoid
  // repeated allocations and setup costs across calls.
  static thread_local ZSTD_CCtx* cctx = nullptr;
  static thread_local bool configured = false;
  if (!cctx) {
    cctx = ZSTD_createCCtx();
    configured = false;
  }
  if (!cctx) {
    return -1;
  }

  // Configure the context once per thread for fastest compression.
  if (!configured) {
    size_t rc = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 1);
    if (ZSTD_isError(rc)) {
      return -1;
    }
    // Keep other defaults (no checksum, standard frame). These are fast and safe.
    configured = true;
  }

  const size_t srcSize = srcPtr->size();
  const size_t dstCap = ZSTD_compressBound(srcSize);
  compressed.resize(dstCap);

  const size_t n = ZSTD_compress2(cctx, compressed.data(), dstCap, srcPtr->data(), srcSize);
  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

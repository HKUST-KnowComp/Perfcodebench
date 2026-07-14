#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  // Thread-local context to avoid repeated allocation
  thread_local ZSTD_CCtx* ctx = ZSTD_createCCtx();
  
  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);
  
  // Use level 1 for maximum speed while maintaining correctness
  const size_t n = ZSTD_compressCCtx(
      ctx,
      compressed.data(), compressed.size(),
      input.data(), input.size(),
      1);
  
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
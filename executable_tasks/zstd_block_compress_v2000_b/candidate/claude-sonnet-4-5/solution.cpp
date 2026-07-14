#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  
  const size_t max_compressed_size = ZSTD_compressBound(input.size());
  compressed.resize(max_compressed_size);
  
  const size_t n = ZSTD_compressCCtx(
      cctx,
      compressed.data(), compressed.size(),
      input.data(), input.size(),
      3);
  
  if (ZSTD_isError(n)) {
    return -1;
  }
  
  compressed.resize(n);
  return static_cast<int>(n);
}
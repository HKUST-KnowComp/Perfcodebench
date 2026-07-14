#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  static thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (cctx == nullptr) {
    return -1;
  }

  const size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  const size_t n = ZSTD_compressCCtx(
      cctx,
      compressed.data(), bound,
      input.data(), input.size(),
      1);

  if (ZSTD_isError(n)) {
    compressed.clear();
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

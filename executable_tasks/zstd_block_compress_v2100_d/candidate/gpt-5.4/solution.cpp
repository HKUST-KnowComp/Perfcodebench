#include "interface.h"

#include "zstd.h"

#include <string>

namespace {

struct ZstdContextHolder {
  ZSTD_CCtx* cctx;

  ZstdContextHolder() : cctx(ZSTD_createCCtx()) {
    if (cctx) {
      ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19);
      ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 0);
      ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 1);
      ZSTD_CCtx_setParameter(cctx, ZSTD_c_dictIDFlag, 0);
    }
  }

  ~ZstdContextHolder() {
    if (cctx) ZSTD_freeCCtx(cctx);
  }

  ZstdContextHolder(const ZstdContextHolder&) = delete;
  ZstdContextHolder& operator=(const ZstdContextHolder&) = delete;
};

static ZstdContextHolder& get_holder() {
  thread_local ZstdContextHolder holder;
  return holder;
}

}  // namespace

int compress_payload(const std::string& input, std::string& compressed) {
  const size_t src_size = input.size();
  const size_t bound = ZSTD_compressBound(src_size);
  compressed.resize(bound);

  ZstdContextHolder& holder = get_holder();
  if (!holder.cctx) {
    return -1;
  }

  const size_t pledged = ZSTD_CCtx_setPledgedSrcSize(holder.cctx, src_size);
  if (ZSTD_isError(pledged)) {
    return -1;
  }

  const size_t n = ZSTD_compress2(
      holder.cctx,
      compressed.data(), bound,
      input.data(), src_size);
  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}

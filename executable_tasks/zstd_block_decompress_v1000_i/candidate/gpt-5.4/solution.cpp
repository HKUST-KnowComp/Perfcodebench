#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, const char* data, std::size_t size) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + size;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  ZSTD_DCtx* dctx = ZSTD_createDCtx();
  if (dctx == nullptr) {
    return 0;
  }

  const std::size_t out_chunk_size = ZSTD_DStreamOutSize();
  char* out_buf = new (std::nothrow) char[out_chunk_size];
  if (out_buf == nullptr) {
    ZSTD_freeDCtx(dctx);
    return 0;
  }

  ZSTD_inBuffer in;
  in.src = compressed.data();
  in.size = compressed.size();
  in.pos = 0;

  uint64_t hash = 1469598103934665603ULL;
  std::size_t total_out = 0;

  while (true) {
    ZSTD_outBuffer out;
    out.dst = out_buf;
    out.size = out_chunk_size;
    out.pos = 0;

    const std::size_t ret = ZSTD_decompressStream(dctx, &out, &in);
    if (ZSTD_isError(ret)) {
      delete[] out_buf;
      ZSTD_freeDCtx(dctx);
      return 0;
    }

    total_out += out.pos;
    if (total_out > decompressed_size) {
      delete[] out_buf;
      ZSTD_freeDCtx(dctx);
      return 0;
    }

    hash = fnv1a_update(hash, out_buf, out.pos);

    if (ret == 0) {
      break;
    }

    if (in.pos == in.size && out.pos == 0) {
      delete[] out_buf;
      ZSTD_freeDCtx(dctx);
      return 0;
    }
  }

  delete[] out_buf;
  ZSTD_freeDCtx(dctx);

  if (total_out != decompressed_size || in.pos != in.size) {
    return 0;
  }

  return hash;
}

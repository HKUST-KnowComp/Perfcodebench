#include "interface.h"
#include "zstd.h"
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  ZSTD_DCtx* dctx = ZSTD_createDCtx();
  if (!dctx) {
    return 0;
  }
  ZSTD_outBuffer out = {nullptr, 0, 0};
  out.dst = new char[decompressed_size];
  out.size = decompressed_size;
  ZSTD_inBuffer in = {compressed.data(), compressed.size(), 0};
  size_t r = ZSTD_decompressStream(dctx, &out, &in);
  if (ZSTD_isError(r) || out.pos != decompressed_size) {
    ZSTD_freeDCtx(dctx);
    delete[] out.dst;
    return 0;
  }
  uint64_t checksum = checksum_bytes(std::string(out.dst, decompressed_size));
  ZSTD_freeDCtx(dctx);
  delete[] out.dst;
  return checksum;
}
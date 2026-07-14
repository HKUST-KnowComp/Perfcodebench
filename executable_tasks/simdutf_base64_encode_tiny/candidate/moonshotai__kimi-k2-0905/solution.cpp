#include "interface.h"

#include <array>
#include <cstdint>
#include <string>

namespace {

constexpr char kTable[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint64_t encode_and_hash(const std::string& in) {
  const size_t len = in.size();
  const size_t out_len = ((len + 2) / 3) * 4;

  /* 128 B input → 172 B output. 256 is plenty. */
  char buf[256];
  char* dst = buf;

  const unsigned char* src = reinterpret_cast<const unsigned char*>(in.data());
  const unsigned char* end = src + len;

  while (src < end) {
    unsigned int b0 = src[0];
    unsigned int b1 = (src + 1 < end) ? src[1] : 0;
    unsigned int b2 = (src + 2 < end) ? src[2] : 0;
    unsigned int triple = (b0 << 16) | (b1 << 8) | b2;

    dst[0] = kTable[(triple >> 18) & 0x3F];
    dst[1] = kTable[(triple >> 12) & 0x3F];
    dst[2] = (src + 1 < end) ? kTable[(triple >> 6) & 0x3F] : '=';
    dst[3] = (src + 2 < end) ? kTable[triple & 0x3F]           : '=';

    src += 3;
    dst += 4;
  }

  /* Fowler-Noll-Vo 64-bit hash */
  uint64_t h = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(buf);
  const unsigned char* e = p + out_len;
  for (; p < e; ++p) {
    h ^= *p;
    h *= 1099511628211ULL;
  }
  return h;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  return encode_and_hash(input);
}
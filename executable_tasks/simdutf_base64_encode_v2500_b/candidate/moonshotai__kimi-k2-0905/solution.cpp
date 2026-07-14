#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

static constexpr char kEnc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/";

uint64_t encode_checksum(const std::string& in) {
  const size_t n = in.size();
  const size_t out_len = ((n + 2) / 3) * 4;
  char out[256];  // task hint says "small"

  // FNV-1a 64-bit constants
  constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t h = FNV_OFFSET;
  size_t o = 0;

  const unsigned char* p = reinterpret_cast<const unsigned char*>(in.data());
  const size_t blocks = n / 3;

  // main body: full triplets → four chars
  for (size_t i = 0; i < blocks; ++i) {
    const uint32_t b0 = p[i*3+0];
    const uint32_t b1 = p[i*3+1];
    const uint32_t b2 = p[i*3+2];
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
    const uint32_t a0 = kEnc[(triple >> 18) & 63];
    const uint32_t a1 = kEnc[(triple >> 12) & 63];
    const uint32_t a2 = kEnc[(triple >> 6 ) & 63];
    const uint32_t a3 = kEnc[ triple        & 63];
    out[o+0] = static_cast<char>(a0);
    out[o+1] = static_cast<char>(a1);
    out[o+2] = static_cast<char>(a2);
    out[o+3] = static_cast<char>(a3);
    o += 4;
  }
  const size_t done = blocks * 3;

  // tail: 0, 1 or 2 bytes left
  size_t rem = n - done;
  uint32_t b0 = 0, b1 = 0, b2 = 0;
  switch (rem) {
    case 2: b1 = p[done+1]; /*fallthrough*/
    case 1: b0 = p[done+0]; break;
  }
  const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
  if (rem) {
    uint32_t a0 = kEnc[(triple >> 18) & 63];
    uint32_t a1 = kEnc[(triple >> 12) & 63];
    uint32_t a2 = (rem == 1) ? '=' : kEnc[(triple >> 6) & 63];
    uint32_t a3 = (rem == 2) ? kEnc[triple & 63] : '=';
    out[o+0] = static_cast<char>(a0);
    out[o+1] = static_cast<char>(a1);
    out[o+2] = static_cast<char>(a2);
    out[o+3] = static_cast<char>(a3);
    o += 4;
  }

  // inline checksum of the out buffer
  for (size_t i = 0; i < o; ++i) {
    h ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
    h *= FNV_PRIME;
  }
  return h;
}
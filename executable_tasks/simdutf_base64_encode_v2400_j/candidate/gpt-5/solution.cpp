#include "interface.h"

#include <cstddef>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const std::size_t n = input.size();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  constexpr uint64_t prime = 1099511628211ULL; // FNV-1a prime

  const std::size_t n_full = n / 3;
  for (std::size_t i = 0; i < n_full; ++i) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = p[1];
    const uint32_t b2 = p[2];
    p += 3;

    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = kTable[(triple >> 6) & 0x3FU];
    const char c3 = kTable[triple & 0x3FU];

    hash = (hash ^ static_cast<unsigned char>(c0)) * prime;
    hash = (hash ^ static_cast<unsigned char>(c1)) * prime;
    hash = (hash ^ static_cast<unsigned char>(c2)) * prime;
    hash = (hash ^ static_cast<unsigned char>(c3)) * prime;
  }

  const std::size_t rem = n % 3;
  if (rem != 0) {
    const uint32_t b0 = p[0];
    const uint32_t b1 = (rem == 2) ? p[1] : 0U;
    const uint32_t triple = (b0 << 16) | (b1 << 8);

    const char c0 = kTable[(triple >> 18) & 0x3FU];
    const char c1 = kTable[(triple >> 12) & 0x3FU];
    const char c2 = (rem == 2) ? kTable[(triple >> 6) & 0x3FU] : '=';
    const char c3 = '=';

    hash = (hash ^ static_cast<unsigned char>(c0)) * prime;
    hash = (hash ^ static_cast<unsigned char>(c1)) * prime;
    hash = (hash ^ static_cast<unsigned char>(c2)) * prime;
    hash = (hash ^ static_cast<unsigned char>(c3)) * prime;
  }

  return hash;
}

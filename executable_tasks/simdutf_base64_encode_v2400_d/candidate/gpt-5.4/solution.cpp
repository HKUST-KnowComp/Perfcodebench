#include "interface.h"

#include <cstdint>
#include <string>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const std::size_t n = input.size();
  const std::size_t out_len = ((n + 2) / 3) * 4;

  std::string out;
  out.resize(out_len);

  const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());
  char* dst = out.data();

  std::size_t i = 0;
  std::size_t j = 0;

  const std::size_t full = n / 3;
  for (std::size_t k = 0; k < full; ++k) {
    const uint32_t b0 = src[i++];
    const uint32_t b1 = src[i++];
    const uint32_t b2 = src[i++];

    dst[j++] = kTable[b0 >> 2];
    dst[j++] = kTable[((b0 & 0x03u) << 4) | (b1 >> 4)];
    dst[j++] = kTable[((b1 & 0x0Fu) << 2) | (b2 >> 6)];
    dst[j++] = kTable[b2 & 0x3Fu];
  }

  const std::size_t rem = n - i;
  if (rem == 1) {
    const uint32_t b0 = src[i];
    dst[j++] = kTable[b0 >> 2];
    dst[j++] = kTable[(b0 & 0x03u) << 4];
    dst[j++] = '=';
    dst[j++] = '=';
  } else if (rem == 2) {
    const uint32_t b0 = src[i];
    const uint32_t b1 = src[i + 1];
    dst[j++] = kTable[b0 >> 2];
    dst[j++] = kTable[((b0 & 0x03u) << 4) | (b1 >> 4)];
    dst[j++] = kTable[(b1 & 0x0Fu) << 2];
    dst[j++] = '=';
  }

  return checksum_bytes(out.data(), out_len);
}

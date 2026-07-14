#include "interface.h"

#include <cstdint>
#include <string>

namespace {

struct Base64Table {
  int8_t t[256];
  Base64Table() {
    for (int i = 0; i < 256; ++i) t[i] = -1; // invalid by default
    // 'A'-'Z' -> 0..25
    for (int c = 'A'; c <= 'Z'; ++c) t[c] = static_cast<int8_t>(c - 'A');
    // 'a'-'z' -> 26..51
    for (int c = 'a'; c <= 'z'; ++c) t[c] = static_cast<int8_t>(c - 'a' + 26);
    // '0'-'9' -> 52..61
    for (int c = '0'; c <= '9'; ++c) t[c] = static_cast<int8_t>(c - '0' + 52);
    t[static_cast<unsigned char>('+')] = 62;
    t[static_cast<unsigned char>('/')] = 63;
    t[static_cast<unsigned char>('=')] = -2; // padding
  }
};

inline uint64_t fnv1a_update(uint64_t hash, uint8_t byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= 1099511628211ULL; // FNV prime
  return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  static const Base64Table table_instance;
  const int8_t* const T = table_instance.t;

  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* const end = p + input.size();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  while (p < end) {
    const int c0 = T[p[0]];
    const int c1 = T[p[1]];
    const int c2 = T[p[2]];
    const int c3 = T[p[3]];

    const uint32_t triple = (static_cast<uint32_t>(c0) << 18) |
                            (static_cast<uint32_t>(c1) << 12) |
                            (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6) |
                            static_cast<uint32_t>((c3 < 0) ? 0 : c3);

    hash = fnv1a_update(hash, static_cast<uint8_t>((triple >> 16) & 0xFFu));
    if (c2 != -2) {
      hash = fnv1a_update(hash, static_cast<uint8_t>((triple >> 8) & 0xFFu));
    }
    if (c3 != -2) {
      hash = fnv1a_update(hash, static_cast<uint8_t>(triple & 0xFFu));
    }

    p += 4;
  }

  return hash;
}

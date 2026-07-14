#include "interface.h"

#include <string>

namespace {

int decode_char(unsigned char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a' + 26;
  if (c >= '0' && c <= '9') return c - '0' + 52;
  if (c == '+') return 62;
  if (c == '/') return 63;
  if (c == '=') return -2;
  return -1;
}

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  std::string out;
  out.reserve((input.size() / 4) * 3);
  for (std::size_t i = 0; i < input.size(); i += 4) {
    const int c0 = decode_char(static_cast<unsigned char>(input[i]));
    const int c1 = decode_char(static_cast<unsigned char>(input[i + 1]));
    const int c2 = decode_char(static_cast<unsigned char>(input[i + 2]));
    const int c3 = decode_char(static_cast<unsigned char>(input[i + 3]));
    const uint32_t triple =
        (static_cast<uint32_t>(c0) << 18) |
        (static_cast<uint32_t>(c1) << 12) |
        (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6) |
        static_cast<uint32_t>((c3 < 0) ? 0 : c3);
    out.push_back(static_cast<char>((triple >> 16) & 0xFFU));
    if (c2 != -2) out.push_back(static_cast<char>((triple >> 8) & 0xFFU));
    if (c3 != -2) out.push_back(static_cast<char>(triple & 0xFFU));
  }
  return checksum_bytes(out);
}

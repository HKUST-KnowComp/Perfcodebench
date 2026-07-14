#include "interface.h"

#include <string>

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  std::string out;
  out.reserve(((input.size() + 2) / 3) * 4);
  for (std::size_t i = 0; i < input.size(); i += 3) {
    const uint32_t b0 = static_cast<unsigned char>(input[i]);
    const uint32_t b1 = (i + 1 < input.size()) ? static_cast<unsigned char>(input[i + 1]) : 0U;
    const uint32_t b2 = (i + 2 < input.size()) ? static_cast<unsigned char>(input[i + 2]) : 0U;
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
    out.push_back(kTable[(triple >> 18) & 0x3FU]);
    out.push_back(kTable[(triple >> 12) & 0x3FU]);
    out.push_back((i + 1 < input.size()) ? kTable[(triple >> 6) & 0x3FU] : '=');
    out.push_back((i + 2 < input.size()) ? kTable[triple & 0x3FU] : '=');
  }
  return checksum_bytes(out);
}

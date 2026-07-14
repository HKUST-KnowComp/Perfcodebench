#include "interface.h"

#include <string>

namespace {

void append_utf8(std::string& out, uint32_t cp) {
  if (cp <= 0x7F) {
    out.push_back(static_cast<char>(cp));
  } else if (cp <= 0x7FF) {
    out.push_back(static_cast<char>(0xC0U | (cp >> 6)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  } else if (cp <= 0xFFFF) {
    out.push_back(static_cast<char>(0xE0U | (cp >> 12)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 6) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  } else {
    out.push_back(static_cast<char>(0xF0U | (cp >> 18)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 12) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 6) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  }
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

uint64_t transcode_checksum(const std::u16string& input) {
  std::string out;
  out.reserve(input.size() * 3);
  for (std::size_t i = 0; i < input.size(); ++i) {
    uint32_t cp = input[i];
    if (cp >= 0xD800U && cp <= 0xDBFFU && i + 1 < input.size()) {
      const uint32_t lo = input[i + 1];
      cp = 0x10000U + (((cp - 0xD800U) << 10) | (lo - 0xDC00U));
      ++i;
    }
    append_utf8(out, cp);
  }
  return checksum_bytes(out);
}

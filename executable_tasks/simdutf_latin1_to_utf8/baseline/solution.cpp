#include "interface.h"

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

uint64_t transcode_checksum(const std::string& input) {
  std::string out;
  out.reserve(input.size() * 2);
  for (unsigned char ch : input) {
    if (ch <= 0x7FU) {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back(static_cast<char>(0xC0U | (ch >> 6)));
      out.push_back(static_cast<char>(0x80U | (ch & 0x3FU)));
    }
  }
  return checksum_bytes(out);
}

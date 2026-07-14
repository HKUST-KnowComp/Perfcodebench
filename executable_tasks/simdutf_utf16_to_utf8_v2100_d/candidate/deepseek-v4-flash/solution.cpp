#include "interface.h"
#include <simdutf.h>
#include <string>

namespace {

uint64_t fnv1a_checksum(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return fnv1a_checksum(std::string());
  }
  // Allocate worst-case buffer (UTF-8 can be up to 3 bytes per UTF-16 code unit)
  std::string out(input.size() * 3, '\0');
  // Transcode using simdutf (assumes valid UTF-16LE input)
  size_t written = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), &out[0]);
  out.resize(written);
  return fnv1a_checksum(out);
}

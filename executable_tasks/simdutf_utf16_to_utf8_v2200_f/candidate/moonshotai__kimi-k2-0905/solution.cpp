#include "interface.h"

#include <simdutf.h>
#include <string>
#include <vector>

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

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) return checksum_bytes("");

  const char16_t* src = input.data();
  size_t src_len = input.size();

  size_t utf8_len = simdutf::utf8_length_from_utf16le(src, src_len);
  std::string out;
  out.resize(utf8_len);

  size_t written = simdutf::convert_utf16le_to_utf8(src, src_len,
                                                    out.data(),
                                                    simdutf::encoding_type::UTF8);
  out.resize(written);

  return checksum_bytes(out);
}
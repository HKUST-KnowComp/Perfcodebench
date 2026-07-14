#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* udata = reinterpret_cast<const unsigned char*>(data);
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(udata[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) return 1469598103934665603ULL;

  // Use simdutf to determine the required size for the UTF-8 output
  // The input is std::u16string, which is UTF-16LE on most platforms where this is tested.
  // We use the convert_utf16le_to_utf8 interface.
  
  size_t utf8_len = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), nullptr, 0);
  
  std::string out;
  out.resize(utf8_len);
  
  simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data(), utf8_len);
  
  return checksum_bytes(out.data(), out.size());
}
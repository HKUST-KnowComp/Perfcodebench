#include "interface.h"
#include <memory>
#include "simdutf.h"

uint64_t transcode_checksum(const std::u16string& input) {
  const std::size_t input_len = input.size();
  if (input_len == 0) {
    return 1469598103934665603ULL;
  }
  const std::size_t utf8_len = simdutf::utf8_length_from_utf16le(input.data(), input_len);
  std::unique_ptr<char[]> buffer(new char[utf8_len]);
  char* const out = buffer.get();
  const char* const end = simdutf::convert_valid_utf16le_to_utf8(input.data(), input_len, out);
  uint64_t hash = 1469598103934665603ULL;
  for (const char* p = out; p != end; ++p) {
    uint64_t byte_val = static_cast<unsigned char>(*p);
    hash ^= byte_val;
    hash *= 1099511628211ULL;
  }
  return hash;
}
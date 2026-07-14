#include "interface.h"
#include "simdutf.h"

#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* in = input.data();
  const size_t inlen = input.size();

  // Compute exact UTF-8 output size and allocate once
  const size_t outlen = simdutf::utf8_length_from_utf16(in, inlen);
  std::string out;
  out.resize(outlen);

  if (outlen != 0) {
    char* dst = &out[0];
    (void)simdutf::convert_utf16_to_utf8(in, inlen, dst);
  }

  // FNV-1a checksum over UTF-8 bytes
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
  for (size_t i = 0; i < outlen; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

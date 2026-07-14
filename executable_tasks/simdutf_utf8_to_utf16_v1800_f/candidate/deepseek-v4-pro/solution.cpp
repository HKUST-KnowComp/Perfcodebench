#include "interface.h"

#include <vector>
#include <simdutf.h>

uint64_t transcode_checksum(const std::string& input) {
  // Maximum possible UTF-16 code units equals input.size() (each ASCII byte becomes one unit)
  std::vector<char16_t> out(input.size());
  char16_t* end = simdutf::utf8_to_utf16le(input.data(), input.size(), out.data());
  const size_t len = end - out.data();

  uint64_t hash = 1469598103934665603ULL;
  const char16_t* p = out.data();
  const char16_t* pe = p + len;
  while (p < pe) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= 1099511628211ULL;
    ++p;
  }
  return hash;
}

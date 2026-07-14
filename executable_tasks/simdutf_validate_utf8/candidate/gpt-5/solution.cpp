#include "interface.h"
#include <simdutf.h>

#include <cstddef>
#include <string>
#include <vector>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  const simdutf::implementation& impl = simdutf::get_active_implementation();
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    const char* data = s.data();
    std::size_t len = s.size();

    // Fast-path: tiny ASCII-only strings are trivially valid UTF-8.
    if (len <= 16) {
      bool ascii = true;
      for (std::size_t i = 0; i < len; ++i) {
        if (static_cast<unsigned char>(data[i]) & 0x80u) { ascii = false; break; }
      }
      if (ascii) {
        ++count;
        continue;
      }
    }

    if (impl.validate_utf8(data, len)) {
      ++count;
    }
  }
  return count;
}

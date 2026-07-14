#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // is_valid_utf8 is expected to be declared in is_utf8.h;
    // adjust the function name if the header uses a different signature
    if (is_valid_utf8(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
      ++count;
    }
  }
  return count;
}

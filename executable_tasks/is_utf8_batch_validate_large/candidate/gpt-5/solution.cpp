#include "interface.h"
#include "is_utf8.h"

namespace {
// SFINAE wrapper to call is_utf8 with either const char* or const unsigned char*
// depending on what the provided header defines.
template <typename Dummy = void>
inline auto call_is_utf8_impl(const char* p, std::size_t n, int) -> decltype(is_utf8(p, n), bool()) {
  return is_utf8(p, n);
}

template <typename Dummy = void>
inline bool call_is_utf8_impl(const char* p, std::size_t n, long) {
  return is_utf8(reinterpret_cast<const unsigned char*>(p), n);
}

inline bool call_is_utf8(const char* p, std::size_t n) {
  return call_is_utf8_impl(p, n, 0);
}
}

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const std::string& s : inputs) {
    count += call_is_utf8(s.data(), s.size()) ? 1u : 0u;
  }
  return count;
}

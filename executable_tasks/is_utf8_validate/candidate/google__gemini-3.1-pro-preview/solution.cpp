#include "interface.h"
#include "is_utf8.h"

template<typename String>
auto check_utf8_impl(const String& s, int) -> decltype(is_utf8(s.data(), s.size())) {
    return is_utf8(s.data(), s.size());
}

template<typename String>
auto check_utf8_impl(const String& s, long) -> decltype(is_utf8(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
    return is_utf8(reinterpret_cast<const unsigned char*>(s.data()), s.size());
}

template<typename String>
auto check_utf8_impl(const String& s, float) -> decltype(is_utf8(s)) {
    return is_utf8(s);
}

template<typename String>
auto check_utf8_impl(const String& s, double) -> decltype(is_utf8(s.c_str())) {
    return is_utf8(s.c_str());
}

bool check_utf8(const std::string& s) {
    return check_utf8_impl(s, 0) ? true : false;
}

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (check_utf8(s)) {
      count++;
    }
  }
  return count;
}

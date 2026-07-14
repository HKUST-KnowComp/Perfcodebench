#include "interface.h"
#include "simdjson.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  const simdjson::implementation* impl = simdjson::get_active_implementation();
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (s.empty()) { count++; continue; }
    count += static_cast<std::size_t>(impl->validate_utf8(s.data(), s.size()) == simdjson::SUCCESS);
  }
  return count;
}

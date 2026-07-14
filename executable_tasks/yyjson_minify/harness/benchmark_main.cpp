#include "interface.h"

#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace {

std::string make_json(std::size_t count) {
  std::ostringstream out;
  out << "[\n";
  for (std::size_t i = 0; i < count; ++i) {
    if (i != 0) {
      out << ",\n";
    }
    out << "  {\n";
    out << "    \"id\": " << i << ",\n";
    out << "    \"flag\": " << ((i % 3) == 0 ? "true" : "false") << ",\n";
    out << "    \"name\": \"item_" << (i % 257) << "\",\n";
    out << "    \"vals\": [ " << i << ", " << (i + 1) << ", " << (i + 2) << " ]\n";
    out << "  }";
  }
  out << "\n]\n";
  return out.str();
}

}  // namespace

int main() {
  const std::string json_input = make_json(14000);
  const std::string expected = nlohmann::ordered_json::parse(json_input).dump();

  const auto start = std::chrono::steady_clock::now();
  const std::string actual = minify_json(json_input);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_size\":" << expected.size() << ","
            << "\"actual_size\":" << actual.size() << "}\n";
  return ok ? 0 : 1;
}

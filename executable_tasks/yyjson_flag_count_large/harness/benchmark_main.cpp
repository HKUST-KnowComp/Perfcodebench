#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string make_json(std::size_t count) {
  std::ostringstream out;
  out << '[';
  for (std::size_t i = 0; i < count; ++i) {
    if (i != 0) {
      out << ',';
    }
    out << "{\"id\":" << i
        << ",\"flag\":" << ((i % 4) == 0 ? "true" : "false")
        << ",\"value\":" << (i * 13 + 5)
        << ",\"name\":\"item_" << (i % 257) << "\"}";
  }
  out << ']';
  return out.str();
}

uint64_t expected_checksum(std::size_t count) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < count; ++i) {
    if ((i % 4) == 0) {
      std::ostringstream name;
      name << "item_" << (i % 257);
      sum += static_cast<uint64_t>(i);
      sum += static_cast<uint64_t>(i * 13 + 5);
      sum += static_cast<uint64_t>(name.str().size());
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 64000;
  const std::string json_input = make_json(kCount);
  const uint64_t expected = expected_checksum(kCount);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = compute_checksum(json_input);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}

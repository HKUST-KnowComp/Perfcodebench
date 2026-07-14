#include "interface.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> make_inputs(std::size_t count) {
  std::vector<std::string> inputs;
  inputs.reserve(count);
  uint64_t state = 0xabcdef1234567890ULL;
  for (std::size_t i = 0; i < count; ++i) {
    std::string s;
    const std::size_t len = 24 + (i % 13);
    s.resize(len);
    for (std::size_t j = 0; j < len; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      s[j] = static_cast<char>(0x20 + (state % 95ULL));
    }
    if ((i % 6) == 0) {
      s[len / 2] = static_cast<char>(0xC0 | (i & 0x1F));
    }
    inputs.push_back(std::move(s));
  }
  return inputs;
}

std::size_t expected_count(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    bool ok = true;
    for (unsigned char ch : s) {
      if (ch > 0x7F) {
        ok = false;
        break;
      }
    }
    count += ok ? 1U : 0U;
  }
  return count;
}

}  // namespace

int main() {
  const std::vector<std::string> inputs = make_inputs(90000);
  const std::size_t expected = expected_count(inputs);

  const auto start = std::chrono::steady_clock::now();
  const std::size_t actual = count_ascii_strings(inputs);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_count\":" << expected << ","
            << "\"actual_count\":" << actual << "}\n";
  return ok ? 0 : 1;
}

#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_values(std::size_t n) {
  std::vector<uint32_t> values(n);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    values[i] = static_cast<uint32_t>((state >> 12) & 0x3ffffu);
  }
  return values;
}

std::vector<uint8_t> encode_varints(const std::vector<uint32_t>& values) {
  std::vector<uint8_t> out;
  out.reserve(values.size() * 3);
  for (uint32_t value : values) {
    while (value >= 128u) {
      out.push_back(static_cast<uint8_t>((value & 127u) | 128u));
      value >>= 7u;
    }
    out.push_back(static_cast<uint8_t>(value));
  }
  return out;
}

uint64_t expected_sum(const std::vector<uint32_t>& values) {
  uint64_t sum = 0;
  for (uint32_t value : values) {
    sum += static_cast<uint64_t>(value);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 2621440u;
  constexpr int kIters = 16;

  const std::vector<uint32_t> values = make_values(kCount);
  const std::vector<uint8_t> encoded = encode_varints(values);
  const uint64_t expected = expected_sum(values);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = varint_sum(encoded, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_sum\":" << expected << ","
            << "\"actual_sum\":" << actual << "}\n";
  return ok ? 0 : 1;
}

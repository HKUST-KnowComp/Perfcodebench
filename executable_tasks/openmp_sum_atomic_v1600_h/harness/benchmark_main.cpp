#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_input(std::size_t n) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<uint32_t>(state % 16384ULL);
  }
  return out;
}

uint64_t expected_sum(const std::vector<uint32_t>& data) {
  uint64_t sum = 0;
  for (uint32_t v : data) {
    sum += static_cast<uint64_t>(v);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 100663296u;
  constexpr int kIters = 1;

  const std::vector<uint32_t> input = make_input(kCount);
  const uint64_t expected = expected_sum(input);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = sum_repeated(input, kIters);
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

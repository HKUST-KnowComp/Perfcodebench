#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_values(std::size_t n) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0x0fedcba987654321ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<uint32_t>(state & 1023u);
  }
  return out;
}

uint64_t checksum_u64(const std::vector<uint64_t>& values) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : values) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<uint32_t>& values, int window) {
  std::vector<uint64_t> out(values.size(), 0);
  for (std::size_t i = 0; i < values.size(); ++i) {
    uint64_t sum = 0;
    const std::size_t begin = (i + 1 < static_cast<std::size_t>(window)) ? 0 : (i + 1 - static_cast<std::size_t>(window));
    for (std::size_t j = begin; j <= i; ++j) {
      sum += static_cast<uint64_t>(values[j]);
    }
    out[i] = sum;
  }
  return checksum_u64(out);
}

}  // namespace

int main() {
  const std::vector<uint32_t> values = make_values(1u << 20);
  constexpr int kWindow = 128;
  constexpr int kIters = 10;
  const uint64_t expected = expected_checksum(values, kWindow);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = window_sum_checksum(values, kWindow, kIters);
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

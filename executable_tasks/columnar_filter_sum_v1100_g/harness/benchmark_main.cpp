#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> keys;
  std::vector<uint32_t> values;
  std::vector<uint8_t> flags;
};

Input make_input(std::size_t n) {
  Input out;
  out.keys.resize(n);
  out.values.resize(n);
  out.flags.resize(n);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.keys[i] = static_cast<uint32_t>((state >> 16) & 8191u);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.values[i] = static_cast<uint32_t>(((state >> 20) & 1023u) + 1u);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.flags[i] = static_cast<uint8_t>((state >> 28) & 1u);
  }
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 12582912u;
  constexpr uint32_t kThreshold = 3072;
  constexpr int kIters = 24;

  const Input input = make_input(kCount);
  const uint64_t expected = expected_sum(input.keys, input.values, input.flags, kThreshold);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = filter_sum(input.keys, input.values, input.flags, kThreshold, kIters);
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

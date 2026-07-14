#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_input(std::size_t n, uint32_t key_space) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0x13579bdf2468ace0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(key_space));
  }
  return out;
}

uint64_t expected_count(const std::vector<uint32_t>& keys, uint32_t key_space) {
  std::vector<uint8_t> seen(static_cast<std::size_t>(key_space), 0);
  uint64_t count = 0;
  for (uint32_t key : keys) {
    if (!seen[static_cast<std::size_t>(key)]) {
      seen[static_cast<std::size_t>(key)] = 1;
      ++count;
    }
  }
  return count;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr uint32_t kKeySpace = 4096;
  constexpr int kIters = 20;

  const std::vector<uint32_t> keys = make_input(kCount, kKeySpace);
  const uint64_t expected = expected_count(keys, kKeySpace);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = distinct_count(keys, kKeySpace, kIters);
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

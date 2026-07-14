#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_input(std::size_t n) {
  std::vector<uint32_t> deltas(n);
  uint64_t state = 0xabcdef0123456789ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    deltas[i] = static_cast<uint32_t>((state & 15u) + 1u);
  }
  return deltas;
}

uint64_t expected_sum(const std::vector<uint32_t>& deltas, uint32_t base) {
  uint64_t sum = 0;
  uint64_t value = base;
  for (uint32_t delta : deltas) {
    value += static_cast<uint64_t>(delta);
    sum += value;
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr uint32_t kBase = 17;
  constexpr int kIters = 6;

  const std::vector<uint32_t> deltas = make_input(kCount);
  const uint64_t expected = expected_sum(deltas, kBase);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = delta_prefix_sum(deltas, kBase, kIters);
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

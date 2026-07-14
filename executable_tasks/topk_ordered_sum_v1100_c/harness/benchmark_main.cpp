#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_input(std::size_t n) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0xabcdef0123456789ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out[i] = static_cast<uint32_t>(state & 1048575u);
  }
  return out;
}

uint64_t expected_sum(const std::vector<uint32_t>& scores, int k) {
  std::vector<uint32_t> tmp = scores;
  std::sort(tmp.begin(), tmp.end(), std::greater<uint32_t>());
  uint64_t sum = 0;
  for (int i = 0; i < k; ++i) {
    sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 20;
  constexpr int kTop = 128;
  constexpr int kIters = 40;

  const std::vector<uint32_t> scores = make_input(kCount);
  const uint64_t expected = expected_sum(scores, kTop);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = topk_sum(scores, kTop, kIters);
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

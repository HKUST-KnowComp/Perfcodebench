#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_sorted_unique(std::size_t n, uint32_t seed_bias) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0xabcdef0123456789ULL + seed_bias;
  uint32_t cur = seed_bias;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    cur += 1u + static_cast<uint32_t>(state % 7ULL);
    out[i] = cur;
  }
  return out;
}

uint64_t expected_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs) {
  uint64_t sum = 0;
  std::size_t i = 0;
  std::size_t j = 0;
  while (i < lhs.size() && j < rhs.size()) {
    if (lhs[i] == rhs[j]) {
      sum += static_cast<uint64_t>(lhs[i]);
      ++i;
      ++j;
    } else if (lhs[i] < rhs[j]) {
      ++i;
    } else {
      ++j;
    }
  }
  return sum;
}

}  // namespace

int main() {
  const std::vector<uint32_t> lhs = make_sorted_unique(1u << 20, 0);
  std::vector<uint32_t> rhs = lhs;
  for (std::size_t i = 0; i < rhs.size(); i += 3) {
    rhs[i] += 1u;
  }
  constexpr int kIters = 24;
  const uint64_t expected = expected_sum(lhs, rhs);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = sorted_intersection_sum(lhs, rhs, kIters);
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

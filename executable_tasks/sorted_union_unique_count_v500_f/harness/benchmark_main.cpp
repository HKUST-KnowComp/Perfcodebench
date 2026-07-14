#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_input(std::size_t n, uint32_t offset) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0x123456789abcdef0ULL + offset;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out[i] = static_cast<uint32_t>((state >> 12) & 0x1fffffu);
  }
  std::sort(out.begin(), out.end());
  return out;
}

uint64_t expected_count(const std::vector<uint32_t>& left, const std::vector<uint32_t>& right) {
  std::size_t i = 0;
  std::size_t j = 0;
  uint64_t count = 0;
  bool has_last = false;
  uint32_t last = 0;
  while (i < left.size() || j < right.size()) {
    uint32_t value = 0;
    if (j == right.size() || (i < left.size() && left[i] <= right[j])) {
      value = left[i++];
    } else {
      value = right[j++];
    }
    if (!has_last || value != last) {
      last = value;
      has_last = true;
      ++count;
    }
  }
  return count;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr int kIters = 6;

  const std::vector<uint32_t> left = make_input(kCount, 0u);
  const std::vector<uint32_t> right = make_input(kCount, 17u);
  const uint64_t expected = expected_count(left, right);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = sorted_union_count(left, right, kIters);
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

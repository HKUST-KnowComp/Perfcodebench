#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace {

void make_ranges(std::vector<uint32_t>& starts, std::vector<uint32_t>& lengths, std::size_t n) {
  starts.resize(n);
  lengths.resize(n);
  uint64_t state = 0x55aa11ee7788cc33ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    const uint32_t base = static_cast<uint32_t>((state >> 12) & ((1U << 20) - 1U));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    starts[i] = base;
    lengths[i] = 8U + static_cast<uint32_t>(state % 320U);
  }
}

uint64_t expected_union(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  std::vector<std::pair<uint32_t, uint32_t>> intervals;
  intervals.reserve(starts.size());
  for (std::size_t i = 0; i < starts.size(); ++i) {
    intervals.emplace_back(starts[i], starts[i] + lengths[i]);
  }
  std::sort(intervals.begin(), intervals.end());
  uint64_t total = 0;
  uint32_t cur_l = 0;
  uint32_t cur_r = 0;
  bool have = false;
  for (const auto& [l, r] : intervals) {
    if (!have) {
      cur_l = l;
      cur_r = r;
      have = true;
      continue;
    }
    if (l > cur_r) {
      total += static_cast<uint64_t>(cur_r - cur_l);
      cur_l = l;
      cur_r = r;
    } else if (r > cur_r) {
      cur_r = r;
    }
  }
  if (have) {
    total += static_cast<uint64_t>(cur_r - cur_l);
  }
  return total;
}

}  // namespace

int main() {
  std::vector<uint32_t> starts;
  std::vector<uint32_t> lengths;
  make_ranges(starts, lengths, 18000);
  const uint64_t expected = expected_union(starts, lengths);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = union_cardinality(starts, lengths);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_cardinality\":" << expected << ","
            << "\"actual_cardinality\":" << actual << "}\n";
  return ok ? 0 : 1;
}

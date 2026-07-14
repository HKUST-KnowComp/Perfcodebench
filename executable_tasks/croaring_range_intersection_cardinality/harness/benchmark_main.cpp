#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace {

void make_ranges(
    std::vector<uint32_t>& starts,
    std::vector<uint32_t>& lengths,
    std::size_t n,
    uint64_t seed) {
  starts.resize(n);
  lengths.resize(n);
  uint64_t state = seed;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    starts[i] = static_cast<uint32_t>((state >> 13) & ((1U << 20) - 1U));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    lengths[i] = 12U + static_cast<uint32_t>(state % 280U);
  }
}

std::vector<std::pair<uint32_t, uint32_t>> merge_ranges(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  std::vector<std::pair<uint32_t, uint32_t>> intervals;
  intervals.reserve(starts.size());
  for (std::size_t i = 0; i < starts.size(); ++i) {
    intervals.emplace_back(starts[i], starts[i] + lengths[i]);
  }
  std::sort(intervals.begin(), intervals.end());
  std::vector<std::pair<uint32_t, uint32_t>> merged;
  for (const auto& [l, r] : intervals) {
    if (merged.empty() || l > merged.back().second) {
      merged.emplace_back(l, r);
    } else if (r > merged.back().second) {
      merged.back().second = r;
    }
  }
  return merged;
}

uint64_t expected_intersection(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  const auto lhs = merge_ranges(lhs_starts, lhs_lengths);
  const auto rhs = merge_ranges(rhs_starts, rhs_lengths);
  std::size_t i = 0;
  std::size_t j = 0;
  uint64_t total = 0;
  while (i < lhs.size() && j < rhs.size()) {
    const uint32_t l = std::max(lhs[i].first, rhs[j].first);
    const uint32_t r = std::min(lhs[i].second, rhs[j].second);
    if (l < r) {
      total += static_cast<uint64_t>(r - l);
    }
    if (lhs[i].second < rhs[j].second) {
      ++i;
    } else {
      ++j;
    }
  }
  return total;
}

}  // namespace

int main() {
  std::vector<uint32_t> lhs_starts;
  std::vector<uint32_t> lhs_lengths;
  std::vector<uint32_t> rhs_starts;
  std::vector<uint32_t> rhs_lengths;
  make_ranges(lhs_starts, lhs_lengths, 15000, 0xabcdef0123456789ULL);
  make_ranges(rhs_starts, rhs_lengths, 15000, 0x13579bdf2468ace0ULL);
  const uint64_t expected =
      expected_intersection(lhs_starts, lhs_lengths, rhs_starts, rhs_lengths);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      intersection_cardinality(lhs_starts, lhs_lengths, rhs_starts, rhs_lengths);
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

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
    uint64_t seed,
    uint32_t mask,
    uint32_t min_len,
    uint32_t len_mod) {
  starts.resize(n);
  lengths.resize(n);
  uint64_t state = seed;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    starts[i] = static_cast<uint32_t>((state >> 11) & mask);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    lengths[i] = min_len + static_cast<uint32_t>(state % len_mod);
  }
}

std::vector<std::pair<uint32_t, uint32_t>> normalize_ranges(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  std::vector<std::pair<uint32_t, uint32_t>> intervals;
  intervals.reserve(starts.size());
  for (std::size_t i = 0; i < starts.size(); ++i) {
    intervals.emplace_back(starts[i], starts[i] + lengths[i]);
  }
  std::sort(intervals.begin(), intervals.end());
  std::vector<std::pair<uint32_t, uint32_t>> merged;
  merged.reserve(intervals.size());
  for (const auto& interval : intervals) {
    if (merged.empty() || interval.first > merged.back().second) {
      merged.push_back(interval);
    } else if (interval.second > merged.back().second) {
      merged.back().second = interval.second;
    }
  }
  return merged;
}

uint64_t difference_cardinality_expected(
    const std::vector<std::pair<uint32_t, uint32_t>>& lhs,
    const std::vector<std::pair<uint32_t, uint32_t>>& rhs) {
  uint64_t total = 0;
  std::size_t j = 0;
  for (const auto& [lhs_begin, lhs_end] : lhs) {
    uint32_t cur = lhs_begin;
    while (j < rhs.size() && rhs[j].second <= cur) {
      ++j;
    }
    std::size_t k = j;
    while (k < rhs.size() && rhs[k].first < lhs_end) {
      if (rhs[k].first > cur) {
        total += static_cast<uint64_t>(rhs[k].first - cur);
      }
      if (rhs[k].second >= lhs_end) {
        cur = lhs_end;
        break;
      }
      if (rhs[k].second > cur) {
        cur = rhs[k].second;
      }
      ++k;
    }
    if (cur < lhs_end) {
      total += static_cast<uint64_t>(lhs_end - cur);
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
  make_ranges(lhs_starts, lhs_lengths, 24000, 0x9b4fa12ce77d3351ULL, (1U << 20) - 1U, 8U, 288U);
  make_ranges(rhs_starts, rhs_lengths, 22000, 0x33d9af5e11bc7824ULL, (1U << 20) - 1U, 8U, 352U);

  const auto lhs = normalize_ranges(lhs_starts, lhs_lengths);
  const auto rhs = normalize_ranges(rhs_starts, rhs_lengths);
  const uint64_t expected =
      difference_cardinality_expected(lhs, rhs) +
      difference_cardinality_expected(rhs, lhs);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      xor_cardinality(lhs_starts, lhs_lengths, rhs_starts, rhs_lengths);
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

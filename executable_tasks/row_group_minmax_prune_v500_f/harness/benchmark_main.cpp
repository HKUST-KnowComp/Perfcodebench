#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> values;
  std::vector<uint32_t> mins;
  std::vector<uint32_t> maxs;
};

Input make_input(std::size_t blocks, uint32_t block_size) {
  Input out;
  out.values.resize(blocks * static_cast<std::size_t>(block_size));
  out.mins.resize(blocks);
  out.maxs.resize(blocks);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t b = 0; b < blocks; ++b) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const uint32_t base = static_cast<uint32_t>((state >> 20) & 65535u);
    uint32_t local_min = UINT32_MAX;
    uint32_t local_max = 0;
    const std::size_t start = b * static_cast<std::size_t>(block_size);
    for (uint32_t i = 0; i < block_size; ++i) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      const uint32_t value = base + static_cast<uint32_t>(state & 63u);
      out.values[start + static_cast<std::size_t>(i)] = value;
      local_min = std::min(local_min, value);
      local_max = std::max(local_max, value);
    }
    out.mins[b] = local_min;
    out.maxs[b] = local_max;
  }
  return out;
}

uint64_t expected_sum(const std::vector<uint32_t>& values, uint32_t low, uint32_t high) {
  uint64_t sum = 0;
  for (uint32_t value : values) {
    if (value >= low && value <= high) {
      sum += static_cast<uint64_t>(value);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kBlocks = 1u << 14;
  constexpr uint32_t kBlockSize = 256;
  constexpr uint32_t kLow = 29984;
  constexpr uint32_t kHigh = 30047;
  constexpr int kIters = 12;

  const Input input = make_input(kBlocks, kBlockSize);
  const uint64_t expected = expected_sum(input.values, kLow, kHigh);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      row_group_range_sum(input.values, input.mins, input.maxs, kLow, kHigh, kBlockSize, kIters);
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

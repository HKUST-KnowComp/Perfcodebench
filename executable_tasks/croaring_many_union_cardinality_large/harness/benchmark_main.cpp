#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void make_shards(
    std::vector<uint32_t>& offsets,
    std::vector<uint32_t>& values,
    std::size_t shard_count,
    std::size_t shard_size) {
  offsets.clear();
  values.clear();
  offsets.reserve(shard_count + 1);
  values.reserve(shard_count * shard_size);
  uint64_t state = 0xa15c3de2794bf680ULL;
  for (std::size_t shard = 0; shard < shard_count; ++shard) {
    offsets.push_back(static_cast<uint32_t>(values.size()));
    std::vector<uint32_t> local;
    local.reserve(shard_size);
    for (std::size_t i = 0; i < shard_size; ++i) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      uint32_t value = static_cast<uint32_t>((state >> 11) & 0x000FFFFFU);
      value += static_cast<uint32_t>((shard & 7U) << 8);
      local.push_back(value);
    }
    std::sort(local.begin(), local.end());
    local.erase(std::unique(local.begin(), local.end()), local.end());
    values.insert(values.end(), local.begin(), local.end());
  }
  offsets.push_back(static_cast<uint32_t>(values.size()));
}

uint64_t expected_union_cardinality(
    const std::vector<uint32_t>& values) {
  std::vector<uint32_t> merged = values;
  std::sort(merged.begin(), merged.end());
  merged.erase(std::unique(merged.begin(), merged.end()), merged.end());
  return static_cast<uint64_t>(merged.size());
}

}  // namespace

int main() {
  std::vector<uint32_t> offsets;
  std::vector<uint32_t> values;
  make_shards(offsets, values, 128, 26000);
  const uint64_t expected = expected_union_cardinality(values);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = many_union_cardinality(offsets, values);
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

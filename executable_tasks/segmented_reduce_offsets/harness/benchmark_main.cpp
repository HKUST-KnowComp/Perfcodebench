#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> segment_ids;
  std::vector<uint32_t> values;
};

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

Input make_input(uint32_t segments) {
  Input out;
  uint64_t state = 0x123456789abcdef0ULL;
  std::size_t total = 0;
  for (uint32_t s = 0; s < segments; ++s) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    total += static_cast<std::size_t>((state & 15u) + 1u);
  }
  out.segment_ids.reserve(total);
  out.values.reserve(total);
  state = 0x123456789abcdef0ULL;
  for (uint32_t s = 0; s < segments; ++s) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const uint32_t len = static_cast<uint32_t>((state & 15u) + 1u);
    for (uint32_t j = 0; j < len; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out.segment_ids.push_back(s);
      out.values.push_back(static_cast<uint32_t>((state >> 16) & 2047u));
    }
  }
  return out;
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& segment_ids,
    const std::vector<uint32_t>& values,
    uint32_t segments) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(segments) + 1, 0);
  std::vector<uint64_t> sums(static_cast<std::size_t>(segments), 0);
  for (std::size_t i = 0; i < segment_ids.size(); ++i) {
    ++offsets[static_cast<std::size_t>(segment_ids[i] + 1)];
    sums[static_cast<std::size_t>(segment_ids[i])] += static_cast<uint64_t>(values[i]);
  }
  for (uint32_t s = 0; s < segments; ++s) {
    offsets[static_cast<std::size_t>(s + 1)] += offsets[static_cast<std::size_t>(s)];
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : offsets) {
    hash = mix(hash, v);
  }
  for (uint64_t v : sums) {
    hash = mix(hash, v);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr uint32_t kSegments = 1u << 17;
  constexpr int kIters = 12;

  const Input input = make_input(kSegments);
  const uint64_t expected = expected_checksum(input.segment_ids, input.values, kSegments);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      segmented_reduce_checksum(input.segment_ids, input.values, kSegments, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}

#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

std::vector<uint32_t> make_offsets(std::size_t segments) {
  std::vector<uint32_t> offsets(segments + 1, 0);
  uint64_t state = 0xfeedfacecafebeefULL;
  for (std::size_t i = 0; i < segments; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    offsets[i + 1] = offsets[i] + static_cast<uint32_t>((state & 31u) + 1u);
  }
  return offsets;
}

uint64_t expected_hash(const std::vector<uint32_t>& offsets) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    for (uint32_t i = offsets[s]; i < offsets[s + 1]; ++i) {
      (void)i;
      hash = mix(hash, static_cast<uint32_t>(s));
    }
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kSegments = 1u << 17;
  constexpr int kIters = 12;

  const std::vector<uint32_t> offsets = make_offsets(kSegments);
  const uint64_t expected = expected_hash(offsets);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = segment_expand_hash(offsets, kIters);
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

#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> cache;
  std::vector<uint32_t> page_ids;
  std::vector<uint32_t> slot_offsets;
};

Input make_input(int num_pages, int page_size, std::size_t lookups) {
  Input out;
  out.cache.resize(static_cast<std::size_t>(num_pages) * static_cast<std::size_t>(page_size));
  out.page_ids.resize(lookups);
  out.slot_offsets.resize(lookups);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < out.cache.size(); ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.cache[i] = static_cast<uint32_t>(state & 65535u);
  }
  for (std::size_t i = 0; i < lookups; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.page_ids[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(num_pages));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.slot_offsets[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(page_size));
  }
  return out;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int page_size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < page_ids.size(); ++i) {
    const std::size_t idx =
        static_cast<std::size_t>(page_ids[i]) * static_cast<std::size_t>(page_size) +
        static_cast<std::size_t>(slot_offsets[i]);
    hash = mix(hash, cache[idx]);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr int kPages = 32768;
  constexpr int kPageSize = 64;
  constexpr std::size_t kLookups = 1u << 23;
  constexpr int kIters = 8;

  const Input input = make_input(kPages, kPageSize, kLookups);
  const uint64_t expected =
      expected_checksum(input.cache, input.page_ids, input.slot_offsets, kPageSize);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      paged_kv_checksum(input.cache, input.page_ids, input.slot_offsets, kPages, kPageSize, kIters);
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

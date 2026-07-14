#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> cache_keys;
  std::vector<uint32_t> request_blocks;
  std::vector<int> request_offsets;
};

Input make_input(int requests, int blocks_per_request, uint32_t hash_space) {
  Input out;
  std::vector<uint8_t> seen(static_cast<std::size_t>(hash_space), 0);
  out.request_offsets.resize(static_cast<std::size_t>(requests) + 1);
  uint64_t state = 0x0fedcba987654321ULL;
  while (out.cache_keys.size() < static_cast<std::size_t>(hash_space / 2)) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const uint32_t key = static_cast<uint32_t>(state % static_cast<uint64_t>(hash_space));
    if (!seen[static_cast<std::size_t>(key)]) {
      seen[static_cast<std::size_t>(key)] = 1;
      out.cache_keys.push_back(key);
    }
  }
  std::sort(out.cache_keys.begin(), out.cache_keys.end());
  out.request_blocks.resize(static_cast<std::size_t>(requests) * static_cast<std::size_t>(blocks_per_request));
  for (int r = 0; r < requests; ++r) {
    out.request_offsets[static_cast<std::size_t>(r)] = r * blocks_per_request;
    for (int b = 0; b < blocks_per_request; ++b) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      const bool should_hit = b < 24 || ((state >> 20) & 3u) != 0u;
      if (should_hit) {
        out.request_blocks[static_cast<std::size_t>(r) * static_cast<std::size_t>(blocks_per_request) +
                           static_cast<std::size_t>(b)] =
            out.cache_keys[static_cast<std::size_t>(state) % out.cache_keys.size()];
      } else {
        out.request_blocks[static_cast<std::size_t>(r) * static_cast<std::size_t>(blocks_per_request) +
                           static_cast<std::size_t>(b)] =
            static_cast<uint32_t>((state + 1u) % static_cast<uint64_t>(hash_space));
      }
    }
  }
  out.request_offsets.back() = requests * blocks_per_request;
  return out;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
    uint64_t prefix = 0;
    for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
      if (!std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<std::size_t>(i)])) {
        break;
      }
      ++prefix;
    }
    hash = mix(hash, prefix);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr int kRequests = 12288;
  constexpr int kBlocksPerRequest = 96;
  constexpr uint32_t kHashSpace = 1u << 21;
  constexpr int kIters = 10;

  const Input input = make_input(kRequests, kBlocksPerRequest, kHashSpace);
  const uint64_t expected =
      expected_checksum(input.cache_keys, input.request_blocks, input.request_offsets);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = prefix_cache_checksum(
      input.cache_keys, input.request_blocks, input.request_offsets, kHashSpace, kIters);
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

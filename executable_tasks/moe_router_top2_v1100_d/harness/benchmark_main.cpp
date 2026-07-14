#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace {

std::vector<int32_t> make_logits(int tokens, int experts) {
  std::vector<int32_t> out(static_cast<std::size_t>(tokens) * static_cast<std::size_t>(experts));
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < out.size(); ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<int32_t>((state % 20001ULL) - 10000);
  }
  return out;
}

bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(const std::vector<int32_t>& logits, int tokens, int experts) {
  uint64_t hash = 1469598103934665603ULL;
  for (int t = 0; t < tokens; ++t) {
    std::pair<int32_t, int> best1{-2147483647 - 1, experts};
    std::pair<int32_t, int> best2{-2147483647 - 1, experts};
    for (int e = 0; e < experts; ++e) {
      const std::pair<int32_t, int> cur{
          logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + static_cast<std::size_t>(e)],
          e};
      if (better(cur, best1)) {
        best2 = best1;
        best1 = cur;
      } else if (better(cur, best2)) {
        best2 = cur;
      }
    }
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1.first)));
    hash = mix(hash, static_cast<uint64_t>(best1.second));
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2.first)));
    hash = mix(hash, static_cast<uint64_t>(best2.second));
  }
  return hash;
}

}  // namespace

int main() {
  constexpr int kTokens = 16384;
  constexpr int kExperts = 32;
  constexpr int kIters = 12;

  const std::vector<int32_t> logits = make_logits(kTokens, kExperts);
  const uint64_t expected = expected_checksum(logits, kTokens, kExperts);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = moe_router_checksum(logits, kTokens, kExperts, kIters);
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

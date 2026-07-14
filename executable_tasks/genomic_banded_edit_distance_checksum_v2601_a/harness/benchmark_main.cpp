#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> make_base(std::size_t count, std::size_t len) {
  static constexpr char kAlpha[] = {'A', 'C', 'G', 'T'};
  std::vector<std::string> out(count, std::string(len, 'A'));
  uint64_t state = 0x1029384756abcdefULL;
  for (std::size_t i = 0; i < count; ++i) {
    for (std::size_t j = 0; j < len; ++j) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      out[i][j] = kAlpha[(state >> 61) & 3u];
    }
  }
  return out;
}

std::vector<std::string> mutate(const std::vector<std::string>& in, int mutations) {
  std::vector<std::string> out = in;
  static constexpr char kAlpha[] = {'A', 'C', 'G', 'T'};
  uint64_t state = 0xfedcba9876543210ULL;
  for (std::size_t i = 0; i < out.size(); ++i) {
    for (int m = 0; m < mutations; ++m) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      const std::size_t pos = static_cast<std::size_t>(state % out[i].size());
      const char cur = out[i][pos];
      char next = cur;
      while (next == cur) {
        state = state * 2862933555777941757ULL + 3037000493ULL;
        next = kAlpha[(state >> 60) & 3u];
      }
      out[i][pos] = next;
    }
  }
  return out;
}

int full_distance(const std::string& a, const std::string& b) {
  const std::size_t n = a.size();
  const std::size_t m = b.size();
  std::vector<int> dp((n + 1u) * (m + 1u), 0);
  for (std::size_t i = 0; i <= n; ++i) dp[i * (m + 1u)] = static_cast<int>(i);
  for (std::size_t j = 0; j <= m; ++j) dp[j] = static_cast<int>(j);
  for (std::size_t i = 1; i <= n; ++i) {
    for (std::size_t j = 1; j <= m; ++j) {
      const int cost = (a[i - 1u] == b[j - 1u]) ? 0 : 1;
      const int del = dp[(i - 1u) * (m + 1u) + j] + 1;
      const int ins = dp[i * (m + 1u) + (j - 1u)] + 1;
      const int sub = dp[(i - 1u) * (m + 1u) + (j - 1u)] + cost;
      int best = del < ins ? del : ins;
      if (sub < best) best = sub;
      dp[i * (m + 1u) + j] = best;
    }
  }
  return dp[n * (m + 1u) + m];
}

uint64_t expected_checksum(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    const int d = full_distance(lhs[i], rhs[i]);
    hash ^= static_cast<uint64_t>(d + 1);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kPairs = 4096;
  constexpr std::size_t kLen = 128;
  constexpr int kMutations = 6;
  constexpr int kBand = 8;
  constexpr int kIters = 5;
  const std::vector<std::string> lhs = make_base(kPairs, kLen);
  const std::vector<std::string> rhs = mutate(lhs, kMutations);
  const uint64_t expected = expected_checksum(lhs, rhs);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = batch_edit_distance_checksum(lhs, rhs, kBand, kIters);
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

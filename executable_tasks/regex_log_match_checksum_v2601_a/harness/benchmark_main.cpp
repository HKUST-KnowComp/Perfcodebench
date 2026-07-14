#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool is_upper(char c) { return c >= 'A' && c <= 'Z'; }
bool is_lower(char c) { return c >= 'a' && c <= 'z'; }
bool is_digit(char c) { return c >= '0' && c <= '9'; }
bool is_hex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

std::vector<std::string> make_records(std::size_t n) {
  std::vector<std::string> out;
  out.reserve(n);
  static constexpr char kLower[] = "abcdefghijklmnopqrstuvwxyz";
  static constexpr char kHex[] = "0123456789ABCDEF";
  uint64_t state = 0x55aa55aa12345678ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    std::string s;
    if ((i % 5u) != 0u) {
      s.push_back(static_cast<char>('A' + ((state >> 61) & 15u) % 26u));
      s.push_back(static_cast<char>('A' + ((state >> 57) & 15u) % 26u));
      s.push_back(static_cast<char>('0' + ((state >> 53) & 7u)));
      s.push_back(static_cast<char>('0' + ((state >> 49) & 7u)));
      s.push_back(static_cast<char>('0' + ((state >> 45) & 7u)));
      s.push_back(static_cast<char>('0' + ((state >> 41) & 7u)));
      s.push_back('-');
      for (int j = 0; j < 6; ++j) {
        s.push_back(kLower[(state >> (j + 8)) % 26u]);
      }
      s.push_back('-');
      for (int j = 0; j < 8; ++j) {
        s.push_back(kHex[(state >> (j * 4)) & 15u]);
      }
    } else {
      s = "bad:";
      for (int j = 0; j < 10; ++j) {
        s.push_back(static_cast<char>('a' + ((state >> (j + 3)) & 15u) % 26u));
      }
    }
    out.push_back(std::move(s));
  }
  return out;
}

bool matches(const std::string& s) {
  if (s.size() != 22u) return false;
  if (!is_upper(s[0]) || !is_upper(s[1])) return false;
  if (!is_digit(s[2]) || !is_digit(s[3]) || !is_digit(s[4]) || !is_digit(s[5])) return false;
  if (s[6] != '-') return false;
  for (int i = 7; i < 13; ++i) if (!is_lower(s[static_cast<std::size_t>(i)])) return false;
  if (s[13] != '-') return false;
  for (int i = 14; i < 22; ++i) {
    if (!is_hex(s[static_cast<std::size_t>(i)])) return false;
  }
  return true;
}

uint64_t expected_checksum(const std::vector<std::string>& records) {
  uint64_t hash = 1469598103934665603ULL;
  for (const std::string& s : records) {
    const uint64_t v = matches(s) ? static_cast<uint64_t>(s[2] + s[14]) : 0u;
    hash ^= v + 1u;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 200000;
  constexpr int kIters = 4;
  const std::vector<std::string> records = make_records(kCount);
  const uint64_t expected = expected_checksum(records);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = regex_log_match_checksum(records, kIters);
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

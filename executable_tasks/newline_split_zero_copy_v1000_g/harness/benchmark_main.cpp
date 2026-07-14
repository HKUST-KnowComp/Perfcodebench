#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

std::string make_text(std::size_t lines) {
  std::string out;
  out.reserve(lines * 64);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < lines; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const std::size_t len = 24 + static_cast<std::size_t>(state % 8ULL);
    for (std::size_t j = 0; j < len; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out.push_back(static_cast<char>('a' + (state % 26ULL)));
    }
    out.push_back('\n');
  }
  return out;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  std::size_t start = 0;
  while (start < text.size()) {
    std::size_t end = text.find('\n', start);
    if (end == std::string::npos) {
      end = text.size();
    }
    const std::size_t len = end - start;
    hash = mix(hash, len);
    hash = mix(hash, len ? static_cast<unsigned char>(text[start]) : 0u);
    start = end + 1;
  }
  return hash;
}

}  // namespace

int main() {
  const std::string text = make_text(1u << 18);
  constexpr int kIters = 16;
  const uint64_t expected = expected_checksum(text);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = newline_split_checksum(text, kIters);
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

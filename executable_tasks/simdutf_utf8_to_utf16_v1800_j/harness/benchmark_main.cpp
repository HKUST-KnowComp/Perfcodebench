#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

void append_utf8(std::string& out, uint32_t cp) {
  if (cp <= 0x7F) {
    out.push_back(static_cast<char>(cp));
  } else if (cp <= 0x7FF) {
    out.push_back(static_cast<char>(0xC0U | (cp >> 6)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  } else if (cp <= 0xFFFF) {
    out.push_back(static_cast<char>(0xE0U | (cp >> 12)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 6) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  } else {
    out.push_back(static_cast<char>(0xF0U | (cp >> 18)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 12) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 6) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  }
}

std::string make_input(std::size_t count) {
  std::string input;
  input.reserve(count * 3);
  uint64_t state = 0x0123456789abcdefULL;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    switch (state % 5ULL) {
      case 0: append_utf8(input, 0x20U + static_cast<uint32_t>(state % 0x5FU)); break;
      case 1: append_utf8(input, 0x00C0U + static_cast<uint32_t>(state % 0x02FFU)); break;
      case 2: append_utf8(input, 0x0800U + static_cast<uint32_t>(state % 0x07FFU)); break;
      case 3: append_utf8(input, 0x4E00U + static_cast<uint32_t>(state % 0x06FFU)); break;
      default: append_utf8(input, 0x1F300U + static_cast<uint32_t>(state % 0x01FFU)); break;
    }
  }
  return input;
}

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::string& input) {
  std::vector<char16_t> out;
  out.reserve(input.size());
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* end = p + input.size();
  while (p < end) {
    uint32_t cp = 0;
    const unsigned char c = *p++;
    if (c <= 0x7F) {
      cp = c;
    } else if ((c >> 5) == 0x6) {
      cp = (static_cast<uint32_t>(c & 0x1FU) << 6) |
           static_cast<uint32_t>(*p++ & 0x3FU);
    } else if ((c >> 4) == 0xE) {
      cp = (static_cast<uint32_t>(c & 0x0FU) << 12) |
           (static_cast<uint32_t>(p[0] & 0x3FU) << 6) |
           static_cast<uint32_t>(p[1] & 0x3FU);
      p += 2;
    } else {
      cp = (static_cast<uint32_t>(c & 0x07U) << 18) |
           (static_cast<uint32_t>(p[0] & 0x3FU) << 12) |
           (static_cast<uint32_t>(p[1] & 0x3FU) << 6) |
           static_cast<uint32_t>(p[2] & 0x3FU);
      p += 3;
    }
    if (cp <= 0xFFFFU) {
      out.push_back(static_cast<char16_t>(cp));
    } else {
      cp -= 0x10000U;
      out.push_back(static_cast<char16_t>(0xD800U + (cp >> 10)));
      out.push_back(static_cast<char16_t>(0xDC00U + (cp & 0x3FFU)));
    }
  }
  return checksum_utf16(out);
}

}  // namespace

int main() {
  const std::string input = make_input(750000);
  const uint64_t expected = expected_checksum(input);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = transcode_checksum(input);
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

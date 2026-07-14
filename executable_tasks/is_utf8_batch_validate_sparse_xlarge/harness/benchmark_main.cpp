#include "interface.h"

#include <chrono>
#include <cstddef>
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

bool is_valid_utf8_scalar(const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* end = p + s.size();
  while (p < end) {
    const unsigned char c = *p++;
    if (c <= 0x7F) continue;
    if (c >= 0xC2 && c <= 0xDF) {
      if (p >= end || (p[0] & 0xC0U) != 0x80U) return false;
      ++p;
      continue;
    }
    if (c == 0xE0) {
      if (end - p < 2 || p[0] < 0xA0 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c >= 0xE1 && c <= 0xEC) {
      if (end - p < 2 || (p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c == 0xED) {
      if (end - p < 2 || p[0] < 0x80 || p[0] > 0x9FU || (p[1] & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c >= 0xEE && c <= 0xEF) {
      if (end - p < 2 || (p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c == 0xF0) {
      if (end - p < 3 || p[0] < 0x90 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    if (c >= 0xF1 && c <= 0xF3) {
      if (end - p < 3 || (p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    if (c == 0xF4) {
      if (end - p < 3 || p[0] < 0x80 || p[0] > 0x8FU || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    return false;
  }
  return true;
}

std::vector<std::string> make_inputs(std::size_t count) {
  std::vector<std::string> inputs;
  inputs.reserve(count);
  uint64_t state = 0x2233445566778899ULL;
  for (std::size_t i = 0; i < count; ++i) {
    std::string s;
    const std::size_t chars = 56 + (i % 37);
    for (std::size_t j = 0; j < chars; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      switch (state % 4ULL) {
        case 0: append_utf8(s, 0x20U + static_cast<uint32_t>(state % 0x5FU)); break;
        case 1: append_utf8(s, 0x00A1U + static_cast<uint32_t>(state % 0x03FFU)); break;
        case 2: append_utf8(s, 0x4E00U + static_cast<uint32_t>(state % 0x07FFU)); break;
        default: append_utf8(s, 0x1F600U + static_cast<uint32_t>(state % 80U)); break;
      }
    }
    if ((i % 127) == 0) {
      s.assign("\xE2\x28\xA1", 3);
    }
    inputs.push_back(std::move(s));
  }
  return inputs;
}

std::size_t expected_count(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) count += is_valid_utf8_scalar(s) ? 1U : 0U;
  return count;
}

}  // namespace

int main() {
  const std::vector<std::string> inputs = make_inputs(90000);
  const std::size_t expected = expected_count(inputs);

  const auto start = std::chrono::steady_clock::now();
  const std::size_t actual = count_valid_strings(inputs);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_count\":" << expected << ","
            << "\"actual_count\":" << actual << "}\n";
  return ok ? 0 : 1;
}

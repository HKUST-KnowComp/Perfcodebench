#pragma once

#include "is_utf8.h"
#include <cstddef>
#include <string>

namespace {

bool is_valid_utf8_range(const unsigned char* start, const unsigned char* end) {
  const unsigned char* p = start;
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

}  // namespace

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* end = data + input.size();
  const unsigned char* line_start = data;

  while (line_start < end) {
    const unsigned char* line_end = line_start;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    if (is_valid_utf8_range(line_start, line_end)) {
      ++count;
    }
    line_start = line_end + 1;
  }

  return count;
}
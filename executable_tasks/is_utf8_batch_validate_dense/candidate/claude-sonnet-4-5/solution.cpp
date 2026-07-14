#include "interface.h"
#include "is_utf8.h"
#include <cstdint>

#ifdef __SSE4_2__
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#ifdef __AVX2__
#include <immintrin.h>
#endif

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  
  for (const auto& s : inputs) {
    if (is_utf8(s.data(), s.size())) {
      ++count;
    }
  }
  
  return count;
}
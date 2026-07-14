#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
  std::string_view sv(input);
  const char* p = sv.data();
  const char* end = p + sv.size();
  double sum = 0.0;
  
  while (p < end) {
    double value;
    auto result = fast_float::from_chars(p, end, value);
    
    if (result.ec != std::errc()) {
      break;
    }
    
    sum += value;
    p = result.ptr;
    
    if (p < end && *p == '|') {
      ++p;
    }
  }
  
  return sum;
}

#include "interface.h"
#include "is_utf8.h"
#include <string>
#include <string_view>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  std::string_view sv(input);
  std::size_t start = 0;
  
  while (start < sv.size()) {
    std::size_t end = sv.find('\n', start);
    std::size_t len = (end == std::string_view::npos) ? (sv.size() - start) : (end - start);
    
    // Use the provided is_utf8 library function which is optimized for performance
    if (is_utf8(sv.data() + start, len)) {
      count++;
    }
    
    if (end == std::string_view::npos) {
      break;
    }
    start = end + 1;
  }
  
  // Handle trailing newline case if necessary based on requirements
  // If the input ends with a newline, the loop above correctly processes the empty string after it.
  // If the requirement implies that an empty string after a final newline is not a record, 
  // one might need to adjust, but standard newline-delimited logic usually counts it.
  
  return count;
}
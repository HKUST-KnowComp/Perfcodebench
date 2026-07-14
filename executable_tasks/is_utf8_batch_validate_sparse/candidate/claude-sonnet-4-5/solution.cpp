#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const std::string* ptr = inputs.data();
  const std::string* end = ptr + inputs.size();
  
  while (ptr != end) {
    count += is_utf8(ptr->data(), ptr->size());
    ++ptr;
  }
  
  return count;
}
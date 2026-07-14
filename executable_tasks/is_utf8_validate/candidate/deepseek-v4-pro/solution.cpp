#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        // Assume is_utf8.h provides a fast validator with signature:
        //   bool is_utf8(const unsigned char* data, size_t length);
        if (is_utf8(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
            ++count;
        }
    }
    return count;
}
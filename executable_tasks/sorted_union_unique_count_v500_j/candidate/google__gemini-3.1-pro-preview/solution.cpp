#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    
    const uint32_t* const l_data = left.data();
    const uint32_t* const r_data = right.data();
    const size_t l_size = left.size();
    const size_t r_size = right.size();

    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        size_t j = 0;
        uint64_t current_count = 0;

        while (i < l_size && j < r_size) {
            uint32_t l_val = l_data[i];
            uint32_t r_val = r_data[j];

            if (l_val < r_val) {
                current_count++;
                i++;
                while (i < l_size && l_data[i] == l_val) i++;
            } else if (r_val < l_val) {
                current_count++;
                j++;
                while (j < r_size && r_data[j] == r_val) j++;
            } else {
                // l_val == r_val
                current_count++;
                i++;
                j++;
                while (i < l_size && l_data[i] == l_val) i++;
                while (j < r_size && r_data[j] == r_val) j++;
            }
        }

        while (i < l_size) {
            uint32_t val = l_data[i];
            current_count++;
            i++;
            while (i < l_size && l_data[i] == val) i++;
        }

        while (j < r_size) {
            uint32_t val = r_data[j];
            current_count++;
            j++;
            while (j < r_size && r_data[j] == val) j++;
        }

        count = current_count;
    }

    return count;
}

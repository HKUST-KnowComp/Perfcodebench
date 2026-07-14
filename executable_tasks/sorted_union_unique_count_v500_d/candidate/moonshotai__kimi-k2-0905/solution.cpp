#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    const uint32_t* const l_data = left.data();
    const uint32_t* const r_data = right.data();
    const size_t l_size = left.size();
    const size_t r_size = right.size();

    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        uint64_t local = 0;

        while (i < l_size && j < r_size) {
            uint32_t lv = l_data[i];
            uint32_t rv = r_data[j];
            if (lv < rv) {
                ++local;
                ++i;
                while (i < l_size && l_data[i] == lv) ++i;
            } else if (rv < lv) {
                ++local;
                ++j;
                while (j < r_size && r_data[j] == rv) ++j;
            } else {
                ++local;
                ++i;
                ++j;
                while (i < l_size && l_data[i] == lv) ++i;
                while (j < r_size && r_data[j] == rv) ++j;
            }
        }

        while (i < l_size) {
            uint32_t lv = l_data[i++];
            ++local;
            while (i < l_size && l_data[i] == lv) ++i;
        }

        while (j < r_size) {
            uint32_t rv = r_data[j++];
            ++local;
            while (j < r_size && r_data[j] == rv) ++j;
        }

        count = local;
    }

    return count;
}
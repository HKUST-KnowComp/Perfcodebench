#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters)
{
    const uint32_t* l = left.data();
    const uint32_t* r = right.data();
    const size_t n_left  = left.size();
    const size_t n_right = right.size();

    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0, j = 0;
        uint64_t local = 0;

        while (i < n_left && j < n_right) {
            if (l[i] < r[j]) {
                ++local;
                uint32_t v = l[i];
                while (++i < n_left && l[i] == v) {}
            } else if (r[j] < l[i]) {
                ++local;
                uint32_t v = r[j];
                while (++j < n_right && r[j] == v) {}
            } else {
                ++local;
                uint32_t v = l[i];
                while (++i < n_left && l[i] == v) {}
                while (++j < n_right && r[j] == v) {}
            }
        }

        while (i < n_left) {
            ++local;
            uint32_t v = l[i];
            while (++i < n_left && l[i] == v) {}
        }

        while (j < n_right) {
            ++local;
            uint32_t v = r[j];
            while (++j < n_right && r[j] == v) {}
        }

        count = local;
    }

    return count;
}
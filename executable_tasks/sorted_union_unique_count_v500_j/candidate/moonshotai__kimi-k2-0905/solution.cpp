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
    size_t nl = left.size();
    size_t nr = right.size();

    size_t i = 0, j = 0;
    uint64_t uniq = 0;

    // single linear pass to count distinct elements in the union
    while (i < nl && j < nr) {
        if (l[i] < r[j]) {
            ++uniq;
            uint32_t v = l[i];
            while (i < nl && l[i] == v) ++i;
        } else if (l[i] > r[j]) {
            ++uniq;
            uint32_t v = r[j];
            while (j < nr && r[j] == v) ++j;
        } else { // equal
            ++uniq;
            uint32_t v = l[i];
            while (i < nl && l[i] == v) ++i;
            while (j < nr && r[j] == v) ++j;
        }
    }
    while (i < nl) {
        ++uniq;
        uint32_t v = l[i];
        while (i < nl && l[i] == v) ++i;
    }
    while (j < nr) {
        ++uniq;
        uint32_t v = r[j];
        while (j < nr && r[j] == v) ++j;
    }

    // only the counter increment remains inside the hot loop
    return uniq * static_cast<uint64_t>(iters);
}
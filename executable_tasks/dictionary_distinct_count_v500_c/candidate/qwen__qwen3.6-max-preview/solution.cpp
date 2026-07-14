#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    
    size_t dict_sz = dictionary.size();
    std::vector<int> code_seen(dict_sz, 0);
    std::vector<std::string_view> unique_views;
    unique_views.reserve(dict_sz);
    uint64_t count = 0;
    int gen = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        ++gen;
        for (uint16_t c : codes) {
            code_seen[c] = gen;
        }
        
        unique_views.clear();
        for (size_t i = 0; i < dict_sz; ++i) {
            if (code_seen[i] == gen) {
                unique_views.push_back(dictionary[i]);
            }
        }
        
        std::sort(unique_views.begin(), unique_views.end());
        auto last = std::unique(unique_views.begin(), unique_views.end());
        count = static_cast<uint64_t>(std::distance(unique_views.begin(), last));
    }
    return count;
}
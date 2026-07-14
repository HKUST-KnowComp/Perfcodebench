#include "interface.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    uint64_t unique_count = 0;
    if (!dictionary.empty() && !codes.empty()) {
        std::unordered_map<std::string_view, int> str_to_id;
        str_to_id.reserve(dictionary.size());
        std::vector<int> code_to_strid(dictionary.size());
        int next_id = 0;

        for (size_t i = 0; i < dictionary.size(); ++i) {
            std::string_view sv(dictionary[i]);
            auto it = str_to_id.find(sv);
            if (it == str_to_id.end()) {
                str_to_id[sv] = next_id;
                code_to_strid[i] = next_id;
                next_id++;
            } else {
                code_to_strid[i] = it->second;
            }
        }

        std::vector<bool> seen(next_id, false);
        int count = 0;
        for (uint16_t c : codes) {
            const int sid = code_to_strid[static_cast<size_t>(c)];
            if (!seen[sid]) {
                seen[sid] = true;
                count++;
            }
        }
        unique_count = static_cast<uint64_t>(count);
    }

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = unique_count;
    }
    return count;
}
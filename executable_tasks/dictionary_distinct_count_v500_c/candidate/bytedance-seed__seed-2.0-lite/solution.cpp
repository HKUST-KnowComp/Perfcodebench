#include "interface.h"

#include <string>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    // Preprocess dictionary once to map each code to a unique ID for its string value
    std::unordered_map<std::string, int> str_to_uid;
    std::vector<int> code_to_uid(dictionary.size());
    int next_uid = 0;
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        auto it = str_to_uid.find(s);
        if (it == str_to_uid.end()) {
            str_to_uid[s] = next_uid;
            code_to_uid[i] = next_uid;
            next_uid++;
        } else {
            code_to_uid[i] = it->second;
        }
    }

    if (next_uid == 0 || codes.empty()) {
        return 0;
    }

    // Generation-based tracking avoids resetting the entire seen array every iteration
    std::vector<int> last_seen(next_uid, 0);
    int current_gen = 0;
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        current_gen++;
        uint64_t current_count = 0;
        for (uint16_t code : codes) {
            const int uid = code_to_uid[static_cast<size_t>(code)];
            if (last_seen[uid] != current_gen) {
                last_seen[uid] = current_gen;
                current_count++;
            }
        }
        count = current_count;
    }

    return count;
}
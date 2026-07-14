#include "interface.h"
#include "simdjson.h"
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded_input(json_input);
    
    uint64_t sum = 0;
    try {
        simdjson::ondemand::document doc = parser.iterate(padded_input);
        simdjson::ondemand::array arr = doc.get_array();

        for (simdjson::ondemand::object obj : arr) {
            // Check 'flag' first to skip unnecessary work
            bool flag = false;
            auto flag_res = obj["flag"].get(flag);
            if (flag_res.error() || !flag) {
                continue;
            }

            // Extract 'tag' length
            std::string_view tag;
            auto tag_res = obj["tag"].get(tag);
            if (tag_res.error()) continue;
            sum += static_cast<uint64_t>(tag.size());

            // Extract 'id'
            uint64_t id;
            auto id_res = obj["id"].get(id);
            if (id_res.error()) continue;
            sum += id;

            // Extract 'value'
            uint64_t value;
            auto value_res = obj["value"].get(value);
            if (value_res.error()) continue;
            sum += value;
        }
    } catch (...) {
        // In case of malformed JSON or unexpected structure, return current sum
        // though the task implies valid input matching the schema.
    }

    return sum;
}
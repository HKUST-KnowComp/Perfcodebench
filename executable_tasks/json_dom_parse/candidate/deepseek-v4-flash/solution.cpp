#include "interface.h"
#include <simdjson.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // Create a padded string copy required by simdjson
    simdjson::padded_string padded(json_input.data(), json_input.size());
    simdjson::ondemand::parser parser;
    uint64_t sum = 0;
    // Parse the JSON array
    simdjson::ondemand::document doc = parser.iterate(padded);
    simdjson::ondemand::array arr = doc.get_array();
    for (auto elem : arr) {
        simdjson::ondemand::object obj = elem.get_object();
        // Access fields directly; assume they exist
        uint64_t id_val = obj["id"];
        uint64_t x_val = obj["x"];
        uint64_t vals1_val = obj["vals"][1];
        sum += id_val;
        sum += x_val;
        sum += vals1_val;
    }
    return sum;
}

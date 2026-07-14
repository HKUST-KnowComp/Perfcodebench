#include "interface.h"
#include "simdjson.h"
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded_json(json_input);
    
    simdjson::ondemand::document doc;
    auto error = parser.parse(padded_json).get(doc);
    if (error) return 0;

    uint64_t sum = 0;
    try {
        simdjson::ondemand::array arr = doc.get_array();
        for (auto item : arr) {
            simdjson::ondemand::object obj = item.get_object();
            
            // Extract 'id'
            sum += obj["id"].get_uint64();
            
            // Extract 'x'
            sum += obj["x"].get_uint64();
            
            // Extract 'vals'[1]
            simdjson::ondemand::array vals = obj["vals"].get_array();
            auto it = vals.begin();
            ++it; // Skip index 0
            sum += it->get_uint64();
        }
    } catch (...) {
        // In case of malformed structure relative to expected schema
        return 0;
    }

    return sum;
}
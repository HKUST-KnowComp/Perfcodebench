#include "interface.h"
#include "simdjson.h"
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded_json(json_input);
    
    simdjson::ondemand::document doc = parser.iterate(padded_json);
    uint64_t sum = 0;

    try {
        simdjson::ondemand::array arr = doc.get_array();
        for (simdjson::ondemand::object obj : arr) {
            sum += obj["id"].get_uint64();
            
            simdjson::ondemand::object nested = obj["nested"].get_object();
            sum += nested["a"].get_uint64();
            sum += nested["b"].get_uint64();
        }
    } catch (...) {
        // In case of malformed JSON or missing fields, return 0 or handle as per requirement.
        // The baseline assumes valid input matching the schema.
        return 0;
    }

    return sum;
}
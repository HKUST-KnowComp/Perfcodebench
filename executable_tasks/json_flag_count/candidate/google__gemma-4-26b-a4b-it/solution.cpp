#include "interface.h"
#include "simdjson.h"
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded_json(json_input);
    
    uint64_t sum = 0;
    try {
        simdjson::ondemand::document doc = parser.iterate(padded_json);
        simdjson::ondemand::array arr = doc.get_array();
        
        for (auto item : arr) {
            // Using ondemand allows us to skip fields we don't care about efficiently
            // We check 'flag' first as it is a boolean
            bool flag = item["flag"].get_bool();
            if (flag) {
                sum += item["id"].get_uint64();
            }
        }
    } catch (...) {
        // In case of malformed JSON or missing fields, return 0 or handle as per requirement
        // Given the task context, we assume valid input matching the schema
        return 0;
    }
    
    return sum;
}
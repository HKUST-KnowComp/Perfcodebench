#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json_input);
    auto doc = parser.iterate(padded);
    
    uint64_t sum = 0;
    for (auto item : doc.get_array()) {
        sum += uint64_t(item["id"].get_uint64());
        sum += uint64_t(item["x"].get_uint64());
        
        auto vals = item["vals"].get_array();
        // Skip the first element, get the second
        vals.at(1);
        sum += uint64_t(vals.at(1).get_uint64());
    }
    return sum;
}
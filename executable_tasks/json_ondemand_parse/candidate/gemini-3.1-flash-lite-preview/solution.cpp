#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json_input);
    auto doc = parser.iterate(padded);
    
    uint64_t sum = 0;
    for (auto item : doc.get_array()) {
        auto obj = item.get_object();
        sum += uint64_t(obj["id"].get_uint64());
        
        auto nested = obj["nested"].get_object();
        sum += uint64_t(nested["a"].get_uint64());
        sum += uint64_t(nested["b"].get_uint64());
    }
    return sum;
}
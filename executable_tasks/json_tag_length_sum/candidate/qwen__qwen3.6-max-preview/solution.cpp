#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    static thread_local simdjson::ondemand::parser parser;
    simdjson::padded_string json_padded(json_input);
    simdjson::ondemand::document doc = parser.iterate(json_padded);
    
    uint64_t sum = 0;
    for (auto element : doc) {
        simdjson::ondemand::object obj = element.get_object();
        if (!obj["flag"].get_bool()) continue;
        
        sum += obj["id"].get_uint64();
        sum += obj["value"].get_uint64();
        sum += obj["tag"].get_string().size();
    }
    return sum;
}
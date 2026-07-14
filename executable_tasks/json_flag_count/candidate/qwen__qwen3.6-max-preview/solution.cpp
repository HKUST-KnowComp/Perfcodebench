#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    thread_local simdjson::ondemand::parser parser;
    simdjson::padded_string padded_json(json_input);
    simdjson::ondemand::document doc = parser.iterate(padded_json);
    
    uint64_t sum = 0;
    for (auto item : doc) {
        bool flag = item["flag"].get_bool().value();
        if (flag) {
            sum += item["id"].get_uint64().value();
        }
    }
    return sum;
}

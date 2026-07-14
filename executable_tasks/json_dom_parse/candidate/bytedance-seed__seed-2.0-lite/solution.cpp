#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    static simdjson::ondemand::parser parser;
    const simdjson::padded_string padded_input(json_input);
    auto doc = parser.iterate(padded_input);
    simdjson::ondemand::array root_array = doc;
    
    uint64_t sum = 0;
    for (auto item : root_array) {
        simdjson::ondemand::object obj = item;
        sum += obj["id"].get_uint64_t().value();
        sum += obj["x"].get_uint64_t().value();
        simdjson::ondemand::array vals = obj["vals"];
        sum += vals.at(1).get_uint64_t().value();
    }
    return sum;
}
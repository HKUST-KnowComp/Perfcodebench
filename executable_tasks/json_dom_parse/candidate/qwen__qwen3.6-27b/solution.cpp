#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json_input);
    uint64_t sum = 0;
    for (auto item : doc.get_array()) {
        sum += item["id"].get_uint64();
        sum += item["x"].get_uint64();
        sum += item["vals"][1].get_uint64();
    }
    return sum;
}
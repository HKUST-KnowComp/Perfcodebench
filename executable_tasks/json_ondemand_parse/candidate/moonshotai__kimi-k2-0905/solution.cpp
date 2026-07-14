#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;
    simdjson::ondemand::parser parser;
    simdjson::ondemand::document doc = parser.iterate(json_input);
    for (auto item : doc.get_array()) {
        sum += item["id"].get_uint64();
        auto nested = item["nested"];
        sum += nested["a"].get_uint64();
        sum += nested["b"].get_uint64();
    }
    return sum;
}
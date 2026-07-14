#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json_input);
    simdjson::ondemand::document doc = parser.iterate(padded);
    uint64_t sum = 0;
    for (simdjson::ondemand::object obj : doc.get_array()) {
        sum += obj["id"].get_uint64().value();
        simdjson::ondemand::object nested = obj["nested"].get_object().value();
        sum += nested["a"].get_uint64().value();
        sum += nested["b"].get_uint64().value();
    }
    return sum;
}
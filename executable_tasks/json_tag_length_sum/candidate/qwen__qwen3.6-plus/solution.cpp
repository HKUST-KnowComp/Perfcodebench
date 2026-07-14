#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::dom::parser parser;
    auto doc = parser.parse(json_input);
    uint64_t sum = 0;
    for (auto obj : doc.get_array()) {
        if (obj["flag"].get_bool().value()) {
            sum += obj["id"].get_uint64().value();
            sum += obj["value"].get_uint64().value();
            sum += obj["tag"].get_string().value().size();
        }
    }
    return sum;
}
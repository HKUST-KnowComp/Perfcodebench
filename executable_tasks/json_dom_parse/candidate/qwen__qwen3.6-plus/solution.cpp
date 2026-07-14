#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::dom::parser parser;
    simdjson::dom::element doc = parser.parse(json_input.data(), json_input.size());
    uint64_t sum = 0;
    for (simdjson::dom::object obj : doc.get_array()) {
        uint64_t id, x, v;
        obj["id"].get(id);
        obj["x"].get(x);
        obj["vals"].get_array().at(1).get(v);
        sum += id + x + v;
    }
    return sum;
}
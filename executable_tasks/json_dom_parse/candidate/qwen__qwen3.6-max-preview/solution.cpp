#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    static thread_local simdjson::dom::parser parser;
    simdjson::dom::array doc = parser.parse(json_input).get_array().value();
    uint64_t sum = 0;
    for (auto item : doc) {
        sum += item["id"].get_uint64().value();
        sum += item["x"].get_uint64().value();
        sum += item["vals"].at(1).get_uint64().value();
    }
    return sum;
}

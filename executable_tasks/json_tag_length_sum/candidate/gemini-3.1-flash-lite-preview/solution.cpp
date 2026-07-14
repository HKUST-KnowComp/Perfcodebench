#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json_input);
    auto doc = parser.iterate(padded);

    uint64_t sum = 0;
    for (auto item : doc.get_array()) {
        auto obj = item.get_object();
        
        bool flag = obj["flag"].get_bool();
        if (!flag) continue;

        uint64_t id = obj["id"].get_uint64();
        std::string_view tag = obj["tag"].get_string();
        uint64_t value = obj["value"].get_uint64();

        sum += id;
        sum += static_cast<uint64_t>(tag.size());
        sum += value;
    }
    return sum;
}
#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    using namespace simdjson;
    uint64_t sum = 0;

    padded_string padded_input(json_input);
    ondemand::parser parser;
    auto document = parser.iterate(padded_input);
    ondemand::array items = document.get_array();

    for (auto element : items) {
        ondemand::object item = element.get_object();
        
        bool flag;
        item["flag"].get(flag);
        if (!flag) continue;

        uint64_t id;
        item["id"].get(id);
        sum += id;

        std::string_view tag;
        item["tag"].get(tag);
        sum += static_cast<uint64_t>(tag.size());

        uint64_t value;
        item["value"].get(value);
        sum += value;
    }

    return sum;
}
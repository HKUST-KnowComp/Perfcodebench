#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    static thread_local simdjson::ondemand::parser parser;
    auto doc = parser.iterate(json_input);
    uint64_t sum = 0;
    for (auto item : doc) {
        sum += uint64_t(item["id"]);
        auto nested = item["nested"];
        sum += uint64_t(nested["a"]);
        sum += uint64_t(nested["b"]);
    }
    return sum;
}

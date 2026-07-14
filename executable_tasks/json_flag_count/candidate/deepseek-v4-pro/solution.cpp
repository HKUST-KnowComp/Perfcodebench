#include "interface.h"
#include <simdjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // Convert to padded string for safe simdjson parsing
    simdjson::padded_string padded(json_input);
    simdjson::ondemand::parser parser;
    auto doc = parser.iterate(padded);
    uint64_t sum = 0;
    // Iterate over array elements, summing id when flag is true
    for (auto item : doc.get_array()) {
        // Implicit conversions will throw on error, matching original behaviour
        if (bool(item["flag"])) {
            sum += uint64_t(item["id"]);
        }
    }
    return sum;
}

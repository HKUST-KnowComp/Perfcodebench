#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    // Reusable parser to avoid reallocation of internal buffers.
    static thread_local simdjson::ondemand::parser parser;
    // Must pad the input for SIMD parsing; padded_string makes a copy.
    simdjson::padded_string padded_json(json_input.data(), json_input.size());
    // Parse the document on-demand.
    simdjson::ondemand::document doc = parser.iterate(padded_json);
    uint64_t sum = 0;
    // Iterate over the top-level array of objects.
    for (auto obj : doc.get_array()) {
        // Get object and extract fields.
        simdjson::ondemand::object o = obj.get_object();
        uint64_t id = o["id"];
        sum += id;
        simdjson::ondemand::object nested = o["nested"].get_object();
        sum += uint64_t(nested["a"]);
        sum += uint64_t(nested["b"]);
    }
    return sum;
}
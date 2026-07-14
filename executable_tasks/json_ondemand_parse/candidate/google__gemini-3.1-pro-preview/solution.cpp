#include "interface.h"
#include "simdjson.h"
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // Use a thread_local parser to reuse internal buffers across multiple calls to compute_checksum.
    // This avoids repeated allocations and improves performance in repeated runs.
    thread_local simdjson::ondemand::parser parser;

    // simdjson requires the input buffer to have padding (SIMDJSON_PADDING).
    // Since the input is a const std::string&, we create a padded copy.
    simdjson::padded_string padded(json_input);

    auto doc = parser.iterate(padded);

    uint64_t sum = 0;

    // Iterate through the JSON array.
    // simdjson::ondemand is optimized for forward-only access.
    for (auto item : doc) {
        // Extract 'id' directly from the object.
        sum += uint64_t(item["id"]);

        // Access the 'nested' object and extract 'a' and 'b'.
        auto nested = item["nested"];
        sum += uint64_t(nested["a"]);
        sum += uint64_t(nested["b"]);
    }

    return sum;
}
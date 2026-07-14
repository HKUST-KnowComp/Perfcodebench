#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    // Create mutable buffer to support RapidJSON's zero-copy in-situ parsing
    std::string buf = json_input;
    rapidjson::Document doc;
    // Parse in-place to minimize heap allocations and data copies
    doc.Parse<rapidjson::kParseInsituFlag>(buf.data());

    uint64_t sum = 0;
    // Iterate over top-level array with identical checksum logic to baseline
    for (auto& item : doc.GetArray()) {
        sum += item["id"].GetUint64();
        auto& nested = item["nested"];
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }
    return sum;
}
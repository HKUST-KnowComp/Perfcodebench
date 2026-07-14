#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    // Create mutable buffer for RapidJSON's zero-copy in-situ parsing
    std::string mutable_buf = json_input;
    rapidjson::Document doc;
    if (doc.Parse(mutable_buf.data()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    for (const auto& item : doc.GetArray()) {
        sum += item["id"].GetUint64();
        const auto& nested = item["nested"];
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }

    return sum;
}
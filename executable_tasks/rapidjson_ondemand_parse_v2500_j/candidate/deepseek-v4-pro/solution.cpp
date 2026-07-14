#include "interface.h"

#include <rapidjson/document.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    std::string input_copy(json_input);
    rapidjson::Document doc;
    doc.Parse(input_copy.c_str());

    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        sum += item["id"].GetUint64();
        const auto& nested = item["nested"];
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }
    return sum;
}
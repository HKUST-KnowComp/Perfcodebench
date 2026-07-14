#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        const auto& obj = item.GetObject();
        sum += obj["id"].GetUint64();
        const auto& nested = obj["nested"].GetObject();
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }
    return sum;
}

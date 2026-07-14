#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str(), json_input.size());

    uint64_t sum = 0;
    for (const auto& item : doc.GetArray()) {
        if (!item["flag"].GetBool()) {
            continue;
        }
        sum += item["id"].GetUint64();
        sum += item["value"].GetUint64();
        sum += static_cast<uint64_t>(item["name"].GetStringLength());
    }
    return sum;
}

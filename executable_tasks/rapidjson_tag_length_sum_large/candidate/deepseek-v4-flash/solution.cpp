#include "interface.h"
#include <rapidjson/document.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        if (!item["flag"].GetBool()) {
            continue;
        }
        sum += item["id"].GetUint64();
        sum += static_cast<uint64_t>(item["tag"].GetStringLength());
        sum += item["value"].GetUint64();
    }
    return sum;
}

#include "interface.h"
#include <rapidjson/document.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    for (const auto& item : doc.GetArray()) {
        const auto& id_val = item.GetMember("id");
        sum += id_val.GetUint64();
        const auto& nested = item.GetMember("nested");
        const auto& a_val = nested.GetMember("a");
        const auto& b_val = nested.GetMember("b");
        sum += a_val.GetUint64();
        sum += b_val.GetUint64();
    }
    return sum;
}
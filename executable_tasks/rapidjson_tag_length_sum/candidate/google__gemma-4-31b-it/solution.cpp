#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Use Parse() which is faster than nlohmann::json::parse
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    if (!doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        if (!item.IsObject()) continue;

        // Check 'flag' first to skip unnecessary lookups
        auto it_flag = item.FindMember("flag");
        if (it_flag == item.MemberEnd() || !it_flag->value.IsBool() || !it_flag->value.GetBool()) {
            continue;
        }

        // Sum 'id'
        auto it_id = item.FindMember("id");
        if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
            sum += it_id->value.GetUint64();
        }

        // Sum 'tag' length without allocating a std::string
        auto it_tag = item.FindMember("tag");
        if (it_tag != item.MemberEnd() && it_tag->value.IsString()) {
            sum += static_cast<uint64_t>(it_tag->value.GetStringLength());
        }

        // Sum 'value'
        auto it_val = item.FindMember("value");
        if (it_val != item.MemberEnd() && it_val->value.IsUint64()) {
            sum += it_val->value.GetUint64();
        }
    }

    return sum;
}
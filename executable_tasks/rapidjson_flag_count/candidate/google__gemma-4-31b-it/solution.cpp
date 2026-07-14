#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Use ParseInsitu if we could modify the string, but since we have const std::string,
    // we use Parse which handles the buffer.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (!doc.IsArray()) return 0;

    for (auto& item : doc.GetArray()) {
        if (!item.IsObject()) continue;

        // Check 'flag' first to skip unnecessary lookups
        auto it_flag = item.FindMember("flag");
        if (it_flag == item.MemberEnd() || !it_flag->value.IsBool() || !it_flag->value.GetBool()) {
            continue;
        }

        // Process 'id'
        auto it_id = item.FindMember("id");
        if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
            sum += it_id->value.GetUint64();
        }

        // Process 'value'
        auto it_val = item.FindMember("value");
        if (it_val != item.MemberEnd() && it_val->value.IsUint64()) {
            sum += it_val->value.GetUint64();
        }

        // Process 'name' length
        auto it_name = item.FindMember("name");
        if (it_name != item.MemberEnd() && it_name->value.IsString()) {
            sum += static_cast<uint64_t>(it_name->value.GetStringLength());
        }
    }

    return sum;
}
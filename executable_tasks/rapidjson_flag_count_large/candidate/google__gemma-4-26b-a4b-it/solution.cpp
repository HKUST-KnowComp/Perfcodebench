#include "interface.h"
#include <rapidjson/document.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Use kParseInsituFlag if we could modify the input, but since it's const std::string&, 
    // we use the standard parse. For even more speed, one could copy to a buffer and use kParseInsituFlag.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    if (!doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        if (!item.IsObject()) continue;

        // Check 'flag' field
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
        auto it_value = item.FindMember("value");
        if (it_value != item.MemberEnd() && it_value->value.IsUint64()) {
            sum += it_value->value.GetUint64();
        }

        // Process 'name' size
        auto it_name = item.FindMember("name");
        if (it_name != item.MemberEnd() && it_name->value.IsString()) {
            sum += static_cast<uint64_t>(it_name->value.GetStringLength());
        }
    }

    return sum;
}
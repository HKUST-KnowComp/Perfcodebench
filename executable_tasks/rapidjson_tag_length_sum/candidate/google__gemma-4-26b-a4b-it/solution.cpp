#include "interface.h"
#include <rapidjson/document.h>
#include <string>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // RapidJSON is significantly faster than nlohmann/json for large datasets.
    // We use a local copy of the string to allow for in-situ parsing if needed,
    // though for this specific task, standard DOM parsing is sufficient and safer.
    rapidjson::Document doc;
    
    // Using Parse<kParseInsituFlag> would be faster but requires a mutable char*
    // Since the interface provides const std::string&, we parse the string directly.
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

        // Process 'tag' length
        auto it_tag = item.FindMember("tag");
        if (it_tag != item.MemberEnd() && it_tag->value.IsString()) {
            sum += static_cast<uint64_t>(it_tag->value.GetStringLength());
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
    }

    return sum;
}
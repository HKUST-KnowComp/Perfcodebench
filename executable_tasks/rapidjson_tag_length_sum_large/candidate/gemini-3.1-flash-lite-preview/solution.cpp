#include "interface.h"
#include "rapidjson/document.h"

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());

    if (doc.HasParseError() || !doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
        const rapidjson::Value& item = doc[i];
        
        // Check if 'flag' exists and is true
        auto it_flag = item.FindMember("flag");
        if (it_flag == item.MemberEnd() || !it_flag->value.GetBool()) {
            continue;
        }

        // Extract 'id', 'tag', and 'value'
        auto it_id = item.FindMember("id");
        auto it_tag = item.FindMember("tag");
        auto it_value = item.FindMember("value");

        if (it_id != item.MemberEnd() && it_tag != item.MemberEnd() && it_value != item.MemberEnd()) {
            sum += it_id->value.GetUint64();
            sum += static_cast<uint64_t>(it_tag->value.GetStringLength());
            sum += it_value->value.GetUint64();
        }
    }
    return sum;
}
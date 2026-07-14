#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
            const rapidjson::Value& item = doc[i];
            if (!item.IsObject()) continue;
            auto flag = item.FindMember("flag");
            if (flag == item.MemberEnd() || !flag->value.IsBool()) continue;
            if (!flag->value.GetBool()) continue;

            auto id = item.FindMember("id");
            if (id != item.MemberEnd() && id->value.IsUint64()) {
                sum += id->value.GetUint64();
            }
            auto tag = item.FindMember("tag");
            if (tag != item.MemberEnd() && tag->value.IsString()) {
                sum += tag->value.GetStringLength();
            }
            auto value = item.FindMember("value");
            if (value != item.MemberEnd() && value->value.IsUint64()) {
                sum += value->value.GetUint64();
            }
        }
    }
    return sum;
}
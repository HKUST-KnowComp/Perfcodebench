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
        
        auto flag_it = item.FindMember("flag");
        if (flag_it != item.MemberEnd() && flag_it->value.IsBool() && flag_it->value.GetBool()) {
            auto id_it = item.FindMember("id");
            auto val_it = item.FindMember("value");
            auto name_it = item.FindMember("name");

            if (id_it != item.MemberEnd() && id_it->value.IsUint64()) {
                sum += id_it->value.GetUint64();
            }
            if (val_it != item.MemberEnd() && val_it->value.IsUint64()) {
                sum += val_it->value.GetUint64();
            }
            if (name_it != item.MemberEnd() && name_it->value.IsString()) {
                sum += static_cast<uint64_t>(name_it->value.GetStringLength());
            }
        }
    }
    return sum;
}
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
        
        // Check if flag exists and is true
        auto itFlag = item.FindMember("flag");
        if (itFlag == item.MemberEnd() || !itFlag->value.GetBool()) {
            continue;
        }

        // Accumulate id and value
        auto itId = item.FindMember("id");
        if (itId != item.MemberEnd()) {
            sum += itId->value.GetUint64();
        }

        auto itValue = item.FindMember("value");
        if (itValue != item.MemberEnd()) {
            sum += itValue->value.GetUint64();
        }

        // Accumulate name length
        auto itName = item.FindMember("name");
        if (itName != item.MemberEnd() && itName->value.IsString()) {
            sum += static_cast<uint64_t>(itName->value.GetStringLength());
        }
    }

    return sum;
}
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
        
        // Check flag
        if (!item.HasMember("flag") || !item["flag"].GetBool()) {
            continue;
        }

        // Accumulate id
        if (item.HasMember("id")) {
            sum += item["id"].GetUint64();
        }

        // Accumulate tag length
        if (item.HasMember("tag") && item["tag"].IsString()) {
            sum += static_cast<uint64_t>(item["tag"].GetStringLength());
        }

        // Accumulate value
        if (item.HasMember("value")) {
            sum += item["value"].GetUint64();
        }
    }

    return sum;
}
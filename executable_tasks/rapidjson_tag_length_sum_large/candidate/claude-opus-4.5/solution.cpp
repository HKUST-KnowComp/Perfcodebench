#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.data(), json_input.size());
    
    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        
        auto flagIt = item.FindMember("flag");
        if (flagIt == item.MemberEnd() || !flagIt->value.GetBool()) {
            continue;
        }
        
        auto idIt = item.FindMember("id");
        auto tagIt = item.FindMember("tag");
        auto valueIt = item.FindMember("value");
        
        sum += idIt->value.GetUint64();
        sum += static_cast<uint64_t>(tagIt->value.GetStringLength());
        sum += valueIt->value.GetUint64();
    }
    
    return sum;
}

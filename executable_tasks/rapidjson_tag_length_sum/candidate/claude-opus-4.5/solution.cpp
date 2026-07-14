#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // Copy to mutable buffer for in-situ parsing (zero-copy strings)
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0');
    
    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());
    
    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        
        // Quick check for flag first
        const auto flagIt = item.FindMember("flag");
        if (flagIt == item.MemberEnd() || !flagIt->value.GetBool()) {
            continue;
        }
        
        // Accumulate id
        const auto idIt = item.FindMember("id");
        if (idIt != item.MemberEnd()) {
            sum += idIt->value.GetUint64();
        }
        
        // Accumulate tag length
        const auto tagIt = item.FindMember("tag");
        if (tagIt != item.MemberEnd()) {
            sum += static_cast<uint64_t>(tagIt->value.GetStringLength());
        }
        
        // Accumulate value
        const auto valueIt = item.FindMember("value");
        if (valueIt != item.MemberEnd()) {
            sum += valueIt->value.GetUint64();
        }
    }
    
    return sum;
}
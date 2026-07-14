#include "interface.h"

#include <rapidjson/document.h>
#include <cstdint>
#include <string>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // Copy input for in-situ parsing (modifies buffer)
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0');
    
    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());
    
    if (!doc.IsArray()) {
        return 0;
    }
    
    uint64_t sum = 0;
    
    // Pre-create key references to avoid repeated construction
    static const rapidjson::Value::StringRefType flagKey("flag", 4);
    static const rapidjson::Value::StringRefType idKey("id", 2);
    static const rapidjson::Value::StringRefType valueKey("value", 5);
    static const rapidjson::Value::StringRefType nameKey("name", 4);
    
    const rapidjson::Value& arr = doc;
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const rapidjson::Value& item = arr[i];
        
        // Check flag field
        auto flagIt = item.FindMember(flagKey);
        if (flagIt == item.MemberEnd() || !flagIt->value.GetBool()) {
            continue;
        }
        
        // Get id
        auto idIt = item.FindMember(idKey);
        if (idIt != item.MemberEnd()) {
            sum += idIt->value.GetUint64();
        }
        
        // Get value
        auto valueIt = item.FindMember(valueKey);
        if (valueIt != item.MemberEnd()) {
            sum += valueIt->value.GetUint64();
        }
        
        // Get name length without creating std::string
        auto nameIt = item.FindMember(nameKey);
        if (nameIt != item.MemberEnd()) {
            sum += static_cast<uint64_t>(nameIt->value.GetStringLength());
        }
    }
    
    return sum;
}
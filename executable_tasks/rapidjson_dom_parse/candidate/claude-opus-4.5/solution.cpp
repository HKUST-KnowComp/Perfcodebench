#include "interface.h"

#include <rapidjson/document.h>
#include <cstring>

uint64_t compute_checksum(const std::string& json_input) {
    // Create a mutable copy for in-situ parsing
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0');
    
    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());
    
    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        sum += item["id"].GetUint64();
        sum += item["x"].GetUint64();
        sum += item["vals"][1].GetUint64();
    }
    
    return sum;
}
#include "interface.h"
#include "rapidjson/document.h"
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str(), json_input.size());
    
    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        if (!item["flag"].GetBool()) continue;
        sum += item["id"].GetUint64();
        sum += item["value"].GetUint64();
        sum += item["tag"].GetStringLength();
    }
    return sum;
}
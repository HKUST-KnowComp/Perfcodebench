#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    
    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& obj = arr[i].GetObject();
        sum += obj["id"].GetUint64();
        sum += obj["x"].GetUint64();
        sum += obj["vals"].GetArray()[1].GetUint64();
    }
    return sum;
}
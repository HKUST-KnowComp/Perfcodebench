#include "interface.h"
#include <rapidjson/document.h>
#include <cstdint>
#include <string>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0');
    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());
    const auto& arr = doc.GetArray();
    uint64_t sum = 0;
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& obj = arr[i].GetObject();
        sum += obj["id"].GetUint64();
        const auto& nested = obj["nested"].GetObject();
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }
    return sum;
}
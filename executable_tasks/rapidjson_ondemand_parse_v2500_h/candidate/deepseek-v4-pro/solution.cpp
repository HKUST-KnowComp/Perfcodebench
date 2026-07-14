#include "interface.h"
#include <rapidjson/document.h>
#include <cstring>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    std::vector<char> buffer(json_input.size() + 1);
    std::memcpy(buffer.data(), json_input.data(), json_input.size());
    buffer[json_input.size()] = '\0';
    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());
    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        sum += item["id"].GetUint64();
        const auto& nested = item["nested"];
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }
    return sum;
}
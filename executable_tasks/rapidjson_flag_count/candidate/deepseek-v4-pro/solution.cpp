#include "interface.h"

#include <rapidjson/document.h>

#include <string>
#include <stdexcept>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        throw std::runtime_error("JSON parse error");
    }
    uint64_t sum = 0;
    const rapidjson::Value& arr = doc;
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& obj = arr[i];
        if (!obj["flag"].GetBool()) continue;
        sum += obj["id"].GetUint64();
        sum += obj["value"].GetUint64();
        sum += static_cast<uint64_t>(obj["name"].GetStringLength());
    }
    return sum;
}

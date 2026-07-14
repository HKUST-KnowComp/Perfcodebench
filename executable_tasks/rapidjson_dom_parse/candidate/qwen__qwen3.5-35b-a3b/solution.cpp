#include "interface.h"

#include <cstdint>
#include <string>
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    if (doc.HasParseError() || !doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    const rapidjson::Value* arr = doc.GetArray();
    const size_t count = doc.Size();

    for (size_t i = 0; i < count; ++i) {
        const rapidjson::Value& item = arr[i];
        sum += item["id"].GetUint64();
        sum += item["x"].GetUint64();
        sum += item["vals"][1].GetUint64();
    }
    return sum;
}
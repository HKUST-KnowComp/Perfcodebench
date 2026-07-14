#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.ParseInsitu(const_cast<char*>(json_input.data()));
    if (!doc.IsArray()) return 0;

    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        if (!item["flag"].GetBool()) continue;
        sum += item["id"].GetUint64();
        sum += item["value"].GetUint64();
        const auto& name = item["name"];
        sum += static_cast<uint64_t>(name.GetStringLength());
    }
    return sum;
}
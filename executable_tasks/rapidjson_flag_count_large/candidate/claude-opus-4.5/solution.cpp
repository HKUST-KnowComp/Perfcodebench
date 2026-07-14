#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // Copy to mutable buffer for in-situ parsing (faster, avoids allocations)
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0');

    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());

    uint64_t sum = 0;
    const auto& arr = doc.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const auto& item = arr[i];
        if (!item["flag"].GetBool()) {
            continue;
        }
        sum += item["id"].GetUint64();
        sum += item["value"].GetUint64();
        sum += static_cast<uint64_t>(item["name"].GetStringLength());
    }
    return sum;
}
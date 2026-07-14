#include "interface.h"

#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <cstdint>
#include <cstring>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    rapidjson::Document doc;
    doc.Parse(json_input.c_str(), json_input.size());

    for (const auto& item : doc.GetArray()) {
        if (!item["flag"].GetBool())
            continue;

        sum += item["id"].GetUint64();
        sum += item["value"].GetUint64();
        sum += item["tag"].GetStringLength();
    }

    return sum;
}
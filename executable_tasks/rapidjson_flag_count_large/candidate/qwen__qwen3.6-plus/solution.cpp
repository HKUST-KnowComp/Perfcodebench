#include "interface.h"

#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str(), json_input.size());
    
    uint64_t sum = 0;
    for (size_t i = 0; i < doc.Size(); ++i) {
        const auto& item = doc[i];
        if (item["flag"].GetBool()) {
            sum += item["id"].GetUint64();
            sum += item["value"].GetUint64();
            sum += static_cast<uint64_t>(item["name"].GetStringLength());
        }
    }
    return sum;
}
#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
        const auto& item = doc[i];
        sum += item["id"].GetUint64();
        sum += item["nested"]["a"].GetUint64();
        sum += item["nested"]["b"].GetUint64();
    }
    return sum;
}

#include "interface.h"
#include <rapidjson/document.h>
uint64_t compute_checksum(const std::string& json_input) {
    const auto doc = rapidjson::Document{};
    auto scratch = const_cast<char*>(json_input.c_str());
    doc.ParseInsitu(scratch);
    uint64_t sum = 0;
    for (const auto &item : doc.GetArray()) {
        sum += item["id"].GetUint64();
        const auto &nested = item["nested"];
        sum += nested["a"].GetUint64() + nested["b"].GetUint64();
    }
    return sum;
}
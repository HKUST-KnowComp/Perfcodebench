#include "interface.h"

#include <rapidjson/document.h>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // In‑situ parsing requires a mutable null‑terminated buffer.
    // We copy the input once to own the buffer.
    std::vector<char> buffer(json_input.size() + 1);
    std::memcpy(buffer.data(), json_input.data(), json_input.size());
    buffer[json_input.size()] = '\0';

    rapidjson::Document doc;
    doc.ParseInsitu(buffer.data());

    // The input is assumed valid; the check is kept only for safety.
    if (doc.HasParseError()) {
        return 0;
    }

    const rapidjson::Value& arr = doc;
    uint64_t sum = 0;
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
        const rapidjson::Value& item = arr[i];
        sum += item["id"].GetUint64();
        const rapidjson::Value& nested = item["nested"];
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }
    return sum;
}
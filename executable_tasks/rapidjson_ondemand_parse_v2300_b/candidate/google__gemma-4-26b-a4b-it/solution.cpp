#include "interface.h"
#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Use kParseInsituFlag if we could modify the string, but since it's const std::string&, 
    // we use the standard Parse method. RapidJSON is still much faster than nlohmann.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                // Extract 'id'
                if (item.HasMember("id") && item["id"].IsUint64()) {
                    sum += item["id"].GetUint64();
                }

                // Extract 'nested.a' and 'nested.b'
                if (item.HasMember("nested") && item["nested"].IsObject()) {
                    const auto& nested = item["nested"];
                    if (nested.HasMember("a") && nested["a"].IsUint64()) {
                        sum += nested["a"].GetUint64();
                    }
                    if (nested.HasMember("b") && nested["b"].IsUint64()) {
                        sum += nested["b"].GetUint64();
                    }
                }
            }
        }
    }

    return sum;
}
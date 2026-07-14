#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <string>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // Use a local copy to allow InSitu parsing if needed, 
    // though for this specific task, standard Document parsing is already much faster than nlohmann.
    // We use rapidjson::Document for high-performance DOM parsing.
    rapidjson::Document doc;
    
    // We use Parse in-situ if we could modify the string, but since we receive const std::string&,
    // we will parse the buffer. RapidJSON is still significantly faster than nlohmann.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    if (!doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
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

    return sum;
}
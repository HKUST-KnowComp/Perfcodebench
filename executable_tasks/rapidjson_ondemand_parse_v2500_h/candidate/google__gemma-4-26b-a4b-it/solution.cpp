#include "interface.h"
#include <rapidjson/document.h>
#include <string>
#include <vector>

// Note: To use InSitu parsing, we need a mutable buffer.
// Since the input is const std::string&, we must copy it once.
// However, RapidJSON's DOM parsing is still significantly faster than nlohmann/json.

uint64_t compute_checksum(const std::string& json_input) {
    // RapidJSON InSitu parsing requires a mutable buffer to avoid extra allocations during parsing.
    // We copy the input once to allow the parser to work in-place.
    std::string buffer = json_input;
    
    rapidjson::Document doc;
    // Parse in-situ to minimize memory overhead
    if (doc.ParseInsitu(&buffer[0]).HasParseError()) {
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
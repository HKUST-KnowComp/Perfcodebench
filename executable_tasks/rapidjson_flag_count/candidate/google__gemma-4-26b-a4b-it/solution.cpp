#include "interface.h"
#include <rapidjson/document.h>
#include <string>
#include <vector>

/**
 * Optimized implementation using RapidJSON.
 * RapidJSON is significantly faster than nlohmann/json for large datasets due to 
 * its DOM-based approach and minimal allocation strategy.
 */
uint64_t compute_checksum(const std::string& json_input) {
    // We create a mutable copy because ParseInsitu is the fastest way to parse,
    // as it points directly into the buffer instead of allocating new strings.
    std::string buffer = json_input;
    rapidjson::Document doc;
    
    // ParseInsitu modifies the buffer to avoid string allocations for keys/values
    if (doc.ParseInsitu(&buffer[0]).HasParseError()) {
        return 0;
    }

    if (!doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        if (!item.IsObject()) {
            continue;
        }

        // Check 'flag' field
        auto flag_it = item.FindMember("flag");
        if (flag_it == item.MemberEnd() || !flag_it->value.IsBool() || !flag_it->value.GetBool()) {
            continue;
        }

        // Process 'id'
        auto id_it = item.FindMember("id");
        if (id_it != item.MemberEnd() && id_it->value.IsUint64()) {
            sum += id_it->value.GetUint64();
        }

        // Process 'value'
        auto val_it = item.FindMember("value");
        if (val_it != item.MemberEnd() && val_it->value.IsUint64()) {
            sum += val_it->value.GetUint64();
        }

        // Process 'name' size
        auto name_it = item.FindMember("name");
        if (name_it != item.MemberEnd() && name_it->value.IsString()) {
            sum += static_cast<uint64_t>(name_it->value.GetStringLength());
        }
    }

    return sum;
}
#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <string>
#include <vector>

/**
 * Optimized implementation using RapidJSON.
 * We use the DOM parser but avoid the overhead of nlohmann/json.
 * For even higher performance, a SAX parser could be used, but 
 * RapidJSON DOM is significantly faster than nlohmann and sufficient here.
 */

uint64_t compute_checksum(const std::string& json_input) {
    // RapidJSON works best with a mutable buffer if using ParseInSitu,
    // but since the input is const std::string&, we use the standard Parse.
    // To optimize, we can copy to a local buffer to allow InSitu if needed,
    // but standard Parse is already much faster than nlohmann.
    
    rapidjson::Document doc;
    // We use kParseNoFlags for standard parsing.
    // If performance is still a bottleneck, we could copy json_input to a 
    // std::vector<char> and use ParseInsitu to avoid string allocations.
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
            auto it_id = item.FindMember("id");
            if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
                sum += it_id->value.GetUint64();
            }

            // Extract 'nested.a' and 'nested.b'
            auto it_nested = item.FindMember("nested");
            if (it_nested != item.MemberEnd() && it_nested->value.IsObject()) {
                const auto& nested = it_nested->value;
                
                auto it_a = nested.FindMember("a");
                if (it_a != nested.MemberEnd() && it_a->value.IsUint64()) {
                    sum += it_a->value.GetUint64();
                }

                auto it_b = nested.FindMember("b");
                if (it_b != nested.MemberEnd() && it_b->value.IsUint64()) {
                    sum += it_b->value.GetUint64();
                }
            }
        }
    }

    return sum;
}

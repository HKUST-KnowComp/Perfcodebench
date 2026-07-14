#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <string>
#include <vector>

/**
 * Optimized implementation using RapidJSON.
 * We use kParseInsituFlag to allow the parser to modify the input buffer directly,
 * avoiding extra string allocations for keys and values.
 */
uint64_t compute_checksum(const std::string& json_input) {
    // RapidJSON's Insitu parsing requires a mutable buffer.
    // We copy the input once to provide this buffer.
    std::string buffer = json_input;
    
    rapidjson::Document doc;
    // kParseInsituFlag allows the parser to point directly into the 'buffer' string.
    if (doc.Parse Insitu(&buffer[0]).HasParseError()) {
        return 0;
    }

    if (!doc.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        if (!item.IsObject()) continue;

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

    return sum;
}
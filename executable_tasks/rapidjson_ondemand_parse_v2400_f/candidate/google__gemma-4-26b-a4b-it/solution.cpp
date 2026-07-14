#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <string>
#include <vector>

using namespace rapidjson;

uint64_t compute_checksum(const std::string& json_input) {
    // We need a mutable buffer for InSitu parsing to avoid extra copies
    // and allow the parser to point directly into the string memory.
    std::string buffer = json_input;
    
    Document d;
    // kParseInsituFlag allows the parser to use the buffer itself for string storage,
    // significantly reducing allocations.
    if (d.Parse Insitu(&buffer[0]).HasParseError()) {
        return 0;
    }

    if (!d.IsArray()) {
        return 0;
    }

    uint64_t sum = 0;
    for (auto& item : d.GetArray()) {
        if (!item.IsObject()) continue;

        // Extract 'id'
        auto it_id = item.FindMember("id");
        if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
            sum += it_id->value.GetUint64();
        }

        // Extract 'nested.a' and 'nested.b'
        auto it_nested = item.FindMember("nested");
        if (it_nested != item.MemberEnd() && it_nested->value.IsObject()) {
            const Value& nested = it_nested->value;
            
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
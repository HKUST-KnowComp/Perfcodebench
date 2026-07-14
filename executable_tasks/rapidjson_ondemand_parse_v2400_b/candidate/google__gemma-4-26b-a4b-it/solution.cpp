#include "interface.h"
#include <rapidjson/document.h>
#include <vector>
#include <string>
#include <cstring>

using namespace rapidjson;

uint64_t compute_checksum(const std::string& json_input) {
    // We need a mutable buffer for InSitu parsing to avoid extra allocations
    std::vector<char> buffer(json_input.begin(), json_input.end());
    
    Document d;
    // kParseInsituFlag allows the parser to point directly into the buffer for strings,
    // reducing memory allocations and copies.
    if (d.ParseInsitu(&buffer[0]).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (d.IsArray()) {
        for (auto& item : d.GetArray()) {
            if (item.IsObject()) {
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
        }
    }

    return sum;
}
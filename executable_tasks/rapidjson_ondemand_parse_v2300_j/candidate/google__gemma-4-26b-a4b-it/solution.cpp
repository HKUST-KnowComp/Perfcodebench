#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Use kParseInsituFlag if we could modify the input, but since it's const std::string&, 
    // we use the standard parse. For maximum performance in a real scenario, 
    // we'd use ParseInsitu on a mutable buffer.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                // Extract 'id'
                auto it_id = item.FindMember("id");
                if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
                    sum += it_id->value.GetUint64();
                }

                // Extract 'nested' object
                auto it_nested = item.FindMember("nested");
                if (it_nested != item.MemberEnd() && it_nested->value.IsObject()) {
                    const auto& nested = it_nested->value;
                    
                    // Extract 'a'
                    auto it_a = nested.FindMember("a");
                    if (it_a != nested.MemberEnd() && it_a->value.IsUint64()) {
                        sum += it_a->value.GetUint64();
                    }

                    // Extract 'b'
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
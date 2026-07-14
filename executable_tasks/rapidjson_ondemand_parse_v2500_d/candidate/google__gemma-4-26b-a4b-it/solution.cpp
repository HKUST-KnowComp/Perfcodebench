#include "interface.h"
#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Using Parse instead of ParseInsitu because json_input is const
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
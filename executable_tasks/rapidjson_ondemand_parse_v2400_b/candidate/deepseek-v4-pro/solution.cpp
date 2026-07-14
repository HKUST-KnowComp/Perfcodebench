#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (const auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                auto it_id = item.FindMember("id");
                if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
                    sum += it_id->value.GetUint64();
                } else if (it_id != item.MemberEnd() && it_id->value.IsInt()) {
                    sum += static_cast<uint64_t>(it_id->value.GetInt());
                }
                auto it_nested = item.FindMember("nested");
                if (it_nested != item.MemberEnd() && it_nested->value.IsObject()) {
                    const auto& nested = it_nested->value;
                    auto it_a = nested.FindMember("a");
                    if (it_a != nested.MemberEnd() && it_a->value.IsUint64()) {
                        sum += it_a->value.GetUint64();
                    } else if (it_a != nested.MemberEnd() && it_a->value.IsInt()) {
                        sum += static_cast<uint64_t>(it_a->value.GetInt());
                    }
                    auto it_b = nested.FindMember("b");
                    if (it_b != nested.MemberEnd() && it_b->value.IsUint64()) {
                        sum += it_b->value.GetUint64();
                    } else if (it_b != nested.MemberEnd() && it_b->value.IsInt()) {
                        sum += static_cast<uint64_t>(it_b->value.GetInt());
                    }
                }
            }
        }
    }
    return sum;
}
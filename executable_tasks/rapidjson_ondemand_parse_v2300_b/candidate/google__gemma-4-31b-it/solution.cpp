#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

using namespace rapidjson;

uint64_t compute_checksum(const std::string& json_input) {
    // Use Document for parsing as it is generally faster than nlohmann for this specific access pattern
    // and we are limited to rapidjson/document.h and nlohmann/json.hpp.
    // To maximize performance, we use the InSitu parser if possible, but since we have a const std::string,
    // we use the standard Parse method.
    Document doc;
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    if (!doc.IsArray()) return 0;

    uint64_t sum = 0;
    for (auto& item : doc.GetArray()) {
        if (!item.IsObject()) continue;

        // Extract 'id'
        auto it_id = item.FindMember("id");
        if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
            sum += it_id->value.GetUint64();
        }

        // Extract 'nested' object
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
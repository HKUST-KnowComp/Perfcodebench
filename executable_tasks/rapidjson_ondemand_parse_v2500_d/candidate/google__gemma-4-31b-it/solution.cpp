#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

using namespace rapidjson;

uint64_t compute_checksum(const std::string& json_input) {
    // Using RapidJSON's Document for simplicity in this specific structure,
    // but since the goal is performance and the structure is a simple array of objects,
    // we use the DOM parser which is significantly faster than nlohmann::json.
    // For even higher performance, a custom Handler with Reader could be used,
    // but Document is usually sufficient for this scale of optimization.
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
        if (it_id != item.MemberEnd() && it_id->value.IsNumber()) {
            sum += it_id->value.GetUint64();
        }

        // Extract 'nested' -> 'a' and 'b'
        auto it_nested = item.FindMember("nested");
        if (it_nested != item.MemberEnd() && it_nested->value.IsObject()) {
            const Value& nested = it_nested->value;
            
            auto it_a = nested.FindMember("a");
            if (it_a != nested.MemberEnd() && it_a->value.IsNumber()) {
                sum += it_a->value.GetUint64();
            }
            
            auto it_b = nested.FindMember("b");
            if (it_b != nested.MemberEnd() && it_b->value.IsNumber()) {
                sum += it_b->value.GetUint64();
            }
        }
    }
    return sum;
}
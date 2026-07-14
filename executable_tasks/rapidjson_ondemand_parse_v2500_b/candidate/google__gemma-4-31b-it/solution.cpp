#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

using namespace rapidjson;

uint64_t compute_checksum(const std::string& json_input) {
    // Use RapidJSON's Document for parsing. 
    // While OnDemand is faster for large files, for small-to-medium nested structures,
    // a fast DOM parse with RapidJSON is already orders of magnitude faster than nlohmann.
    // To maximize performance, we use the Document with a stack allocator.
    Document doc;
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (!doc.IsArray()) return 0;

    for (auto& item : doc.GetArray()) {
        if (item.IsObject()) {
            // Use FindMember instead of operator[] to avoid multiple lookups
            auto it_id = item.FindMember("id");
            if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
                sum += it_id->value.GetUint64();
            }

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
    return sum;
}
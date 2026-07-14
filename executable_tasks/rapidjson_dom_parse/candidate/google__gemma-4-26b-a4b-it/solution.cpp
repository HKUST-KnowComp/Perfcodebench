#include "interface.h"
#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Parse the input string. RapidJSON is significantly faster than nlohmann/json.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                // Direct access via FindMember is faster than operator[] which performs more checks/allocations
                auto it_id = item.FindMember("id");
                if (it_id != item.MemberEnd() && it_id->value.IsUint64()) {
                    sum += it_id->value.GetUint64();
                }

                auto it_x = item.FindMember("x");
                if (it_x != item.MemberEnd() && it_x->value.IsUint64()) {
                    sum += it_x->value.GetUint64();
                }

                auto it_vals = item.FindMember("vals");
                if (it_vals != item.MemberEnd() && it_vals->value.IsArray()) {
                    const auto& vals = it_vals->value;
                    if (vals.Size() > 1 && vals[1].IsUint64()) {
                        sum += vals[1].GetUint64();
                    }
                }
            }
        }
    }

    return sum;
}
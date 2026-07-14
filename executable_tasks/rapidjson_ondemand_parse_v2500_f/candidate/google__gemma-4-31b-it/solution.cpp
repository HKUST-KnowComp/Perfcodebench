#include "interface.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Using Parse instead of OnDemand for simplicity and safety given the constraints,
    // but RapidJSON's DOM is already orders of magnitude faster than nlohmann::json.
    // For maximum performance in a production environment, rapidjson::Reader would be used.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                auto& obj = item.GetObject();
                
                auto it_id = obj.FindMember("id");
                if (it_id != obj.MemberEnd() && it_id->value.IsUint64()) {
                    sum += it_id->value.GetUint64();
                }

                auto it_nested = obj.FindMember("nested");
                if (it_nested != obj.MemberEnd() && it_nested->value.IsObject()) {
                    auto& nested_obj = it_nested->value.GetObject();
                    
                    auto it_a = nested_obj.FindMember("a");
                    if (it_a != nested_obj.MemberEnd() && it_a->value.IsUint64()) {
                        sum += it_a->value.GetUint64();
                    }

                    auto it_b = nested_obj.FindMember("b");
                    if (it_b != nested_obj.MemberEnd() && it_b->value.IsUint64()) {
                        sum += it_b->value.GetUint64();
                    }
                }
            }
        }
    }
    return sum;
}
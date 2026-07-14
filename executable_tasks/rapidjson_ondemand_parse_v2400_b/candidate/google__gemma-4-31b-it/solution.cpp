#include "interface.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::CKParseResult result;
    rapidjson::OnDemandParser parser;
    // Use the input string directly to avoid copies
    parser.Parse(json_input.c_str(), json_input.size());

    uint64_t sum = 0;
    rapidjson::OnDemand::Value root = parser.GetRoot();

    if (root.IsArray()) {
        auto array = root.GetArray();
        while (array.HasNext()) {
            rapidjson::OnDemand::Value item = array.MoveNext();
            if (item.IsObject()) {
                auto obj = item.GetObject();
                
                // We need to find 'id', 'nested' (and its children 'a', 'b')
                // Since the order of keys in JSON is not guaranteed, we iterate
                // through the object members.
                bool found_id = false;
                bool found_a = false;
                bool found_b = false;

                auto member = obj.GetMember();
                while (member.HasNext()) {
                    const char* key = member.GetName();
                    rapidjson::OnDemand::Value val = member.GetValue();

                    if (std::strcmp(key, "id") == 0) {
                        sum += val.GetUint64();
                        found_id = true;
                    } else if (std::strcmp(key, "nested") == 0) {
                        if (val.IsObject()) {
                            auto nested_obj = val.GetObject();
                            auto nested_member = nested_obj.GetMember();
                            while (nested_member.HasNext()) {
                                const char* n_key = nested_member.GetName();
                                rapidjson::OnDemand::Value n_val = nested_member.GetValue();
                                if (std::strcmp(n_key, "a") == 0) {
                                    sum += n_val.GetUint64();
                                    found_a = true;
                                } else if (std::strcmp(n_key, "b") == 0) {
                                    sum += n_val.GetUint64();
                                    found_b = true;
                                }
                                nested_member.MoveNext();
                            }
                        }
                    }
                    member.MoveNext();
                }
            }
        }
    }

    return sum;
}
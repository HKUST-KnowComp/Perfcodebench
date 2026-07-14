#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Use kParseInsituFlag if the input string could be modified, 
    // but since we receive const std::string&, we use standard parse.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                // Access 'id'
                if (item.HasMember("id") && item["id"].IsUint64()) {
                    sum += item["id"].GetUint64();
                }
                
                // Access 'x'
                if (item.HasMember("x") && item["x"].IsUint64()) {
                    sum += item["x"].GetUint64();
                }

                // Access 'vals'[1]
                if (item.HasMember("vals") && item["vals"].IsArray()) {
                    const auto& vals = item["vals"];
                    if (vals.Size() > 1 && vals[1].IsUint64()) {
                        sum += vals[1].GetUint64();
                    }
                }
            }
        }
    }

    return sum;
}
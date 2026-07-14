#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    // Parse the JSON input. Using Parse() on the string content.
    if (doc.Parse(json_input.c_str()).HasParseError()) {
        return 0;
    }

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (auto& item : doc.GetArray()) {
            if (item.IsObject()) {
                // Access "id"
                if (item.HasMember("id") && item["id"].IsNumber()) {
                    sum += item["id"].GetUint64();
                }
                // Access "x"
                if (item.HasMember("x") && item["x"].IsNumber()) {
                    sum += item["x"].GetUint64();
                }
                // Access "vals"[1]
                if (item.HasMember("vals") && item["vals"].IsArray() && item["vals"].Size() > 1) {
                    if (item["vals"][1].IsNumber()) {
                        sum += item["vals"][1].GetUint64();
                    }
                }
            }
        }
    }
    return sum;
}
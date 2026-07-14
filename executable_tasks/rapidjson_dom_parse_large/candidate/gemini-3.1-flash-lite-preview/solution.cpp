#include "interface.h"
#include "rapidjson/document.h"

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
            const rapidjson::Value& item = doc[i];
            
            // Accessing fields directly using RapidJSON's optimized lookup
            sum += item["id"].GetUint64();
            sum += item["x"].GetUint64();
            
            // Accessing the second element of the "vals" array
            const rapidjson::Value& vals = item["vals"];
            sum += vals[1].GetUint64();
        }
    }
    return sum;
}
#include "interface.h"
#include "rapidjson/document.h"

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
            const auto& item = doc[i];
            // RapidJSON's GetUint64() is highly efficient for numeric fields
            sum += item["id"].GetUint64();
            sum += item["x"].GetUint64();
            sum += item["vals"][1].GetUint64();
        }
    }
    return sum;
}
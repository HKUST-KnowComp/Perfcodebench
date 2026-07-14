#include "interface.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include "rapidjson/internal/strfunc.h"
#include "rapidjson/memorystream.h"
#include "rapidjson/encodedstream.h"

// Using RapidJSON's Document for standard parsing as it is generally faster than nlohmann/json
// for this specific access pattern.
#include "rapidjson/document.h"

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());

    uint64_t sum = 0;
    if (doc.IsArray()) {
        for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
            const auto& item = doc[i];
            sum += item["id"].GetUint64();
            const auto& nested = item["nested"];
            sum += nested["a"].GetUint64();
            sum += nested["b"].GetUint64();
        }
    }
    return sum;
}
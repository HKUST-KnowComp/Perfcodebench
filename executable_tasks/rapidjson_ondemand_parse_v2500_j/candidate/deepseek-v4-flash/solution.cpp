#include "interface.h"
#include <rapidjson/document.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    using namespace rapidjson;
    Document doc;
    doc.Parse(json_input.c_str());
    uint64_t sum = 0;
    for (Value::ConstValueIterator itr = doc.Begin(); itr != doc.End(); ++itr) {
        const Value& obj = *itr;
        sum += obj["id"].GetUint64();
        sum += obj["nested"]["a"].GetUint64();
        sum += obj["nested"]["b"].GetUint64();
    }
    return sum;
}

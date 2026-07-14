#include "interface.h"
#include <rapidjson/document.h>

namespace {
    inline uint64_t extract_uint64(const rapidjson::Value& v) {
        // Handle all integer types that RapidJSON may produce.
        if (v.IsUint64()) return v.GetUint64();
        if (v.IsInt64())  return static_cast<uint64_t>(v.GetInt64());
        if (v.IsUint())   return v.GetUint();
        if (v.IsInt())    return static_cast<uint64_t>(v.GetInt());
        // Fallback for double (should not occur in correct input).
        return static_cast<uint64_t>(v.GetDouble());
    }
}

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Document doc;
    doc.Parse(json_input.c_str());

    const auto& arr = doc.GetArray();
    uint64_t sum = 0;

    for (const auto& obj : arr) {
        sum += extract_uint64(obj["id"]);
        sum += extract_uint64(obj["x"]);
        sum += extract_uint64(obj["vals"][1]);
    }

    return sum;
}
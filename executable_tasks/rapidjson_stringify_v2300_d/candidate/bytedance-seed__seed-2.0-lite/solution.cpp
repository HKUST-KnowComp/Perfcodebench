#include "interface.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    const size_t n = ids.size();
    rapidjson::StringBuffer buffer;
    // Pre-reserve buffer space to avoid costly reallocations (128 bytes per entry is a safe overhead estimate)
    buffer.Reserve(n * 128);
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartArray();
    for (size_t i = 0; i < n; ++i) {
        writer.StartObject();
        // Write fields in exact required order to match baseline output specification
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool((ids[i] % 3U) == 0U);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        // Pass precomputed string length to avoid redundant strlen() call
        writer.String(names[i].data(), names[i].size());
        writer.EndObject();
    }
    writer.EndArray();

    // Use buffer's exact size to avoid extra strlen() when creating output string
    return std::string(buffer.GetString(), buffer.GetSize());
}
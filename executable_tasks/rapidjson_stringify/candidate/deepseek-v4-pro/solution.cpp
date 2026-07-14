#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace {
inline int count_digits(uint32_t n) {
    if (n == 0) return 1;
    int d = 0;
    while (n) {
        n /= 10;
        ++d;
    }
    return d;
}
} // anonymous namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    // Precompute exact required buffer size to avoid reallocations.
    std::size_t total = 2; // '[' and ']'
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (i > 0) total += 1; // comma between objects
        total += 32;           // static overhead per object (braces, keys, commas, quotes)
        total += count_digits(ids[i]);
        total += count_digits(values[i]);
        total += ((ids[i] % 3U) == 0U) ? 4 : 5; // "true" or "false"
        total += names[i].size();
    }

    rapidjson::StringBuffer buffer;
    buffer.Reserve(total);
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartArray();
    for (std::size_t i = 0; i < n; ++i) {
        writer.StartObject();
        writer.Key("id");
        writer.Uint(ids[i]);
        writer.Key("flag");
        writer.Bool((ids[i] % 3U) == 0U);
        writer.Key("value");
        writer.Uint(values[i]);
        writer.Key("name");
        writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].size()));
        writer.EndObject();
    }
    writer.EndArray();

    return std::string(buffer.GetString(), buffer.GetSize());
}
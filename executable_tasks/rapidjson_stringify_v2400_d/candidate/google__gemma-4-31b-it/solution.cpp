#include "interface.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    // Estimate initial capacity to reduce reallocations
    // Each object: "{\"id\":123456789,\"flag\":true,\"value\":123456789,\"name\":\"some_name\"},"
    // Roughly 60-100 bytes per element
    size_t estimated_size = ids.size() * 80;
    rapidjson::StringBuffer buffer;
    buffer.Reserve(estimated_size);

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartArray();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        writer.StartObject();
        
        writer.Key("id");
        writer.Uint(ids[i]);
        
        writer.Key("flag");
        writer.Bool((ids[i] % 3U) == 0U);
        
        writer.Key("value");
        writer.Uint(values[i]);
        
        writer.Key("name");
        writer.String(names[i].c_str());
        
        writer.EndObject();
    }
    writer.EndArray();

    return std::string(buffer.GetString(), buffer.GetSize());
}
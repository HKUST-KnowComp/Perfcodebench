#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  rapidjson::StringBuffer s;
  // Rough pre-allocation to avoid many buffer resizes.
  // Each record adds at least the braces, colons, commas, keys, and values.
  // Names dominate the size; allocate extra for possible escaping.
  size_t estimated = 2;                         // "[" ... "]"
  estimated += ids.size() * 50;                 // overhead per object & number digits
  for (const auto& name : names) {
    estimated += name.size() * 2;   // pessimistic escape expansion
  }
  s.Reserve(estimated);

  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartArray();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    writer.StartObject();

    writer.Key("id");
    writer.Uint(ids[i]);

    writer.Key("flag");
    writer.Bool((ids[i] % 3u) == 0u);

    writer.Key("value");
    writer.Uint(values[i]);

    writer.Key("name");
    writer.String(names[i].c_str(), static_cast<rapidjson::SizeType>(names[i].size()), false);

    writer.EndObject();
  }
  writer.EndArray();

  return std::string(s.GetString(), s.GetSize());
}
#include "interface.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  using namespace rapidjson;

  StringBuffer sb;
  // Pre-allocate buffer with conservative estimate for performance
  std::size_t est = ids.size() * 60 + 2;
  if (est > sb.GetCapacity()) {
    sb.Reserve(est);
  }

  Writer<StringBuffer> writer(sb);
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
    writer.String(names[i].data(), static_cast<SizeType>(names[i].size()), false);
    writer.EndObject();
  }
  writer.EndArray();

  return std::string(sb.GetString(), sb.GetSize());
}
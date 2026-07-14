#include "interface.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

static std::size_t approx_json_size(const std::vector<uint32_t>& ids,
                                    const std::vector<uint32_t>& /*values*/,
                                    const std::vector<std::string>& names) {
  std::size_t total = 0;
  for (const auto& n : names) {
    total += n.size();
  }
  // overhead per object: }] opening/closing + keys + colons + commas +
  // array brackets and commas   see analysis in comments
  const std::size_t n = ids.size();
  const std::size_t per_obj_overhead = 58;
  total += n * per_obj_overhead + 1;
  return total;
}

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
  rapidjson::StringBuffer buffer;
  std::size_t estimate = approx_json_size(ids, values, names);
  if (estimate > 0) {
    buffer.Reserve(estimate);
  }

  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  writer.StartArray();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    writer.StartObject();

    writer.Key("id", 2);
    writer.Uint(ids[i]);

    writer.Key("flag", 4);
    writer.Bool((ids[i] % 3U) == 0U);

    writer.Key("value", 5);
    writer.Uint(values[i]);

    const auto& name = names[i];
    writer.Key("name", 4);
    writer.String(name.data(), static_cast<rapidjson::SizeType>(name.size()));

    writer.EndObject();
  }
  writer.EndArray();

  return std::string(buffer.GetString(), buffer.GetSize());
}
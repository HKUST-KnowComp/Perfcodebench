#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  rapidjson::StringBuffer sb;
  rapidjson::Document d;
  d.SetObject();
  rapidjson::Value arr(rapidjson::kArrayType);
  d.AddMember("array", arr, d.GetAllocator());
  rapidjson::Value* array = d.FindMember("array")
                         ->value.GetArrayPtr();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    rapidjson::Value obj(rapidjson::kObjectType);
    obj.AddMember("id", ids[i], d.GetAllocator());
    obj.AddMember("flag", (ids[i] % 3U) == 0U, d.GetAllocator());
    obj.AddMember("value", values[i], d.GetAllocator());
    obj.AddMember("name", rapidjson::Value(names[i].c_str(), names[i].length(), d.GetAllocator()), d.GetAllocator());
    array->PushBack(obj, d.GetAllocator());
  }
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  d.Accept(writer);
  return sb.GetString();
}
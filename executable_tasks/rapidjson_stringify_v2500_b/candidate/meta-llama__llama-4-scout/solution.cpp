#include "interface.h"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  rapidjson::StringBuffer buffer;
  rapidjson::Document document;
  document.SetObject();
  rapidjson::Value array(rapidjson::kArrayType);
  document.AddMember("array", array, document.GetAllocator());
  rapidjson::Value obj(rapidjson::kObjectType);
  rapidjson::Value id, flag, value, name;
  for (std::size_t i = 0; i < ids.size(); ++i) {
    obj.SetObject();
    id.SetUint(ids[i]);
    obj.AddMember("id", id, document.GetAllocator());
    flag.SetBool(ids[i] % 3U == 0U);
    obj.AddMember("flag", flag, document.GetAllocator());
    value.SetUint(values[i]);
    obj.AddMember("value", value, document.GetAllocator());
    name.SetString(names[i].c_str(), static_cast<rapidjson::Size>(names[i].size()));
    obj.AddMember("name", name, document.GetAllocator());
    array.PushBack(obj, document.GetAllocator());
  }
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  document.Accept(writer);
  return sb.GetString();
}
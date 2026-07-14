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
  for (std::size_t i = 0; i < ids.size(); ++i) {
    obj.SetObject();
    obj.AddMember("id", rapidjson::Value(ids[i]), document.GetAllocator());
    obj.AddMember("flag", rapidjson::Value((ids[i] % 3U) == 0U), document.GetAllocator());
    obj.AddMember("value", rapidjson::Value(values[i]), document.GetAllocator());
    obj.AddMember("name", rapidjson::Value(names[i].c_str(), names[i].length(), document.GetAllocator()), document.GetAllocator());
    array.PushBack(obj, document.GetAllocator());
  }
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  return buffer.GetString();
}
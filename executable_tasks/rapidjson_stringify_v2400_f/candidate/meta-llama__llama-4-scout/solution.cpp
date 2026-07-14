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
  rapidjson::Value::ArrayType& arr = document["array"].GetArray();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    rapidjson::Value obj(rapidjson::kObjectType);
    obj.AddMember("id", rapidjson::Value(ids[i]).Move(), document.GetAllocator());
    obj.AddMember("flag", rapidjson::Value((ids[i] % 3U) == 0U).Move(), document.GetAllocator());
    obj.AddMember("value", rapidjson::Value(values[i]).Move(), document.GetAllocator());
    obj.AddMember("name", rapidjson::Value(names[i].c_str(), names[i].size(), document.GetAllocator()).Move(), document.GetAllocator());
    arr.PushBack(obj.Move(), document.GetAllocator());
  }
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  return buffer.GetString();
}
#include "interface.h"

#include <string>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string minify_json(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  rapidjson::StringBuffer buffer;
  buffer.Reserve(json_input.size());
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return std::string(buffer.GetString(), buffer.GetSize());
}

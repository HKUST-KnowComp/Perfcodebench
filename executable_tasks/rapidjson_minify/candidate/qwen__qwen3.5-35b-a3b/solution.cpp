#include "interface.h"
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string minify_json(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  doc.Accept(writer);
  return std::string(sb.GetString(), sb.GetSize());
}
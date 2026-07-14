#include "interface.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <stdexcept>

std::string minify_json(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  if (doc.HasParseError()) {
    throw std::runtime_error("Invalid JSON input");
  }

  rapidjson::StringBuffer sb;
  sb.Reserve(json_input.size());
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  doc.Accept(writer);

  return std::string(sb.GetString(), sb.GetSize());
}

#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document document;
  document.Parse(json_input.c_str());
  uint64_t sum = 0;
  if (document.IsArray()) {
    for (rapidjson::SizeType i = 0; i < document.Size(); ++i) {
      if (document[i].HasMember("id") && document[i]["id"].IsUint64()) {
        sum += document[i]["id"].GetUint64();
      }
      if (document[i].HasMember("nested") && document[i]["nested"].IsObject()) {
        if (document[i]["nested"].HasMember("a") && document[i]["nested"]["a"].IsUint64()) {
          sum += document[i]["nested"]["a"].GetUint64();
        }
        if (document[i]["nested"].HasMember("b") && document[i]["nested"]["b"].IsUint64()) {
          sum += document[i]["nested"]["b"].GetUint64();
        }
      }
    }
  }
  return sum;
}
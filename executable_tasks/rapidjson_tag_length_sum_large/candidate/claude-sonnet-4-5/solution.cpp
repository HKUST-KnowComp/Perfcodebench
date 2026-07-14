#include "interface.h"

#include <rapidjson/document.h>

#include <string>
#include <cstring>

uint64_t compute_checksum(const std::string& json_input) {
  // Create a mutable copy for in-situ parsing
  std::string mutable_input = json_input;
  
  rapidjson::Document doc;
  doc.ParseInsitu(&mutable_input[0]);
  
  uint64_t sum = 0;
  
  if (doc.IsArray()) {
    for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
      const rapidjson::Value& item = doc[i];
      
      // Check flag field
      auto flag_it = item.FindMember("flag");
      if (flag_it == item.MemberEnd() || !flag_it->value.IsBool() || !flag_it->value.GetBool()) {
        continue;
      }
      
      // Get id
      auto id_it = item.FindMember("id");
      if (id_it != item.MemberEnd() && id_it->value.IsUint64()) {
        sum += id_it->value.GetUint64();
      }
      
      // Get tag length
      auto tag_it = item.FindMember("tag");
      if (tag_it != item.MemberEnd() && tag_it->value.IsString()) {
        sum += static_cast<uint64_t>(tag_it->value.GetStringLength());
      }
      
      // Get value
      auto value_it = item.FindMember("value");
      if (value_it != item.MemberEnd() && value_it->value.IsUint64()) {
        sum += value_it->value.GetUint64();
      }
    }
  }
  
  return sum;
}
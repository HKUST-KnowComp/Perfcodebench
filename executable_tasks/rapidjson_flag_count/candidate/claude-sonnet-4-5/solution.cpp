#include "interface.h"

#include <rapidjson/document.h>

#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  // Make a mutable copy for in-situ parsing
  std::string mutable_input = json_input;
  
  rapidjson::Document doc;
  doc.ParseInsitu(&mutable_input[0]);
  
  uint64_t sum = 0;
  
  for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
    const rapidjson::Value& item = doc[i];
    
    // Check flag field
    auto flag_it = item.FindMember("flag");
    if (flag_it == item.MemberEnd() || !flag_it->value.GetBool()) {
      continue;
    }
    
    // Get id
    auto id_it = item.FindMember("id");
    if (id_it != item.MemberEnd()) {
      sum += id_it->value.GetUint64();
    }
    
    // Get value
    auto value_it = item.FindMember("value");
    if (value_it != item.MemberEnd()) {
      sum += value_it->value.GetUint64();
    }
    
    // Get name length
    auto name_it = item.FindMember("name");
    if (name_it != item.MemberEnd()) {
      sum += static_cast<uint64_t>(name_it->value.GetStringLength());
    }
  }
  
  return sum;
}
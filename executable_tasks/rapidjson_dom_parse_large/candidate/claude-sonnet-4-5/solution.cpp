#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  
  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }
  
  uint64_t sum = 0;
  const auto& arr = doc.GetArray();
  
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const auto& item = arr[i];
    
    if (item.IsObject()) {
      auto id_it = item.FindMember("id");
      if (id_it != item.MemberEnd() && id_it->value.IsUint64()) {
        sum += id_it->value.GetUint64();
      }
      
      auto x_it = item.FindMember("x");
      if (x_it != item.MemberEnd() && x_it->value.IsUint64()) {
        sum += x_it->value.GetUint64();
      }
      
      auto vals_it = item.FindMember("vals");
      if (vals_it != item.MemberEnd() && vals_it->value.IsArray()) {
        const auto& vals_arr = vals_it->value.GetArray();
        if (vals_arr.Size() > 1 && vals_arr[1].IsUint64()) {
          sum += vals_arr[1].GetUint64();
        }
      }
    }
  }
  
  return sum;
}
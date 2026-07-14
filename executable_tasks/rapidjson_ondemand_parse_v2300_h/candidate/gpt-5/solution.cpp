#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;

  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  if (!doc.IsArray()) {
    return 0;
  }

  for (auto& v : doc.GetArray()) {
    if (!v.IsObject()) continue;
    const auto obj = v.GetObject();

    // id
    auto itId = obj.FindMember("id");
    if (itId != obj.MemberEnd()) {
      const auto& val = itId->value;
      if (val.IsUint64()) sum += val.GetUint64();
      else if (val.IsInt64()) sum += static_cast<uint64_t>(val.GetInt64());
      else if (val.IsUint()) sum += val.GetUint();
      else if (val.IsInt()) sum += static_cast<uint64_t>(val.GetInt());
    }

    // nested.a and nested.b
    auto itNested = obj.FindMember("nested");
    if (itNested != obj.MemberEnd() && itNested->value.IsObject()) {
      const auto nestedObj = itNested->value.GetObject();

      auto itA = nestedObj.FindMember("a");
      if (itA != nestedObj.MemberEnd()) {
        const auto& valA = itA->value;
        if (valA.IsUint64()) sum += valA.GetUint64();
        else if (valA.IsInt64()) sum += static_cast<uint64_t>(valA.GetInt64());
        else if (valA.IsUint()) sum += valA.GetUint();
        else if (valA.IsInt()) sum += static_cast<uint64_t>(valA.GetInt());
      }

      auto itB = nestedObj.FindMember("b");
      if (itB != nestedObj.MemberEnd()) {
        const auto& valB = itB->value;
        if (valB.IsUint64()) sum += valB.GetUint64();
        else if (valB.IsInt64()) sum += static_cast<uint64_t>(valB.GetInt64());
        else if (valB.IsUint()) sum += valB.GetUint();
        else if (valB.IsInt()) sum += static_cast<uint64_t>(valB.GetInt());
      }
    }
  }

  return sum;
}

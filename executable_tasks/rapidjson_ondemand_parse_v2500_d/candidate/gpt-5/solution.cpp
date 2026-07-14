#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  // Parse with known length to avoid strlen
  doc.Parse(json_input.data(), json_input.size());
  if (!doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  const auto arr = doc.GetArray();
  for (auto& v : arr) {
    if (!v.IsObject()) continue;

    const rapidjson::Value& obj = v;

    // Locate members "id" and "nested" in a single scan
    const rapidjson::Value* idValPtr = nullptr;
    const rapidjson::Value* nestedPtr = nullptr;

    for (auto m = obj.MemberBegin(), end = obj.MemberEnd(); m != end; ++m) {
      const auto& name = m->name;
      if (!name.IsString()) continue;
      const char* s = name.GetString();
      const size_t len = name.GetStringLength();
      if (len == 2 && s[0] == 'i' && s[1] == 'd') {
        idValPtr = &m->value;
      } else if (len == 6 && s[0] == 'n' && s[1] == 'e' && s[2] == 's' && s[3] == 't' && s[4] == 'e' && s[5] == 'd') {
        nestedPtr = &m->value;
      }
    }

    if (idValPtr) {
      const auto& idVal = *idValPtr;
      if (idVal.IsUint64()) {
        sum += idVal.GetUint64();
      } else if (idVal.IsUint()) {
        sum += static_cast<uint64_t>(idVal.GetUint());
      } else if (idVal.IsInt64()) {
        const int64_t x = idVal.GetInt64();
        if (x >= 0) sum += static_cast<uint64_t>(x);
      } else if (idVal.IsInt()) {
        const int x = idVal.GetInt();
        if (x >= 0) sum += static_cast<uint64_t>(x);
      }
    }

    if (nestedPtr && nestedPtr->IsObject()) {
      const rapidjson::Value& nested = *nestedPtr;
      const rapidjson::Value* aValPtr = nullptr;
      const rapidjson::Value* bValPtr = nullptr;

      for (auto m = nested.MemberBegin(), end = nested.MemberEnd(); m != end; ++m) {
        const auto& name = m->name;
        if (!name.IsString()) continue;
        const char* s = name.GetString();
        const size_t len = name.GetStringLength();
        if (len == 1) {
          if (s[0] == 'a') {
            aValPtr = &m->value;
          } else if (s[0] == 'b') {
            bValPtr = &m->value;
          }
        }
      }

      if (aValPtr) {
        const auto& aval = *aValPtr;
        if (aval.IsUint64()) {
          sum += aval.GetUint64();
        } else if (aval.IsUint()) {
          sum += static_cast<uint64_t>(aval.GetUint());
        } else if (aval.IsInt64()) {
          const int64_t x = aval.GetInt64();
          if (x >= 0) sum += static_cast<uint64_t>(x);
        } else if (aval.IsInt()) {
          const int x = aval.GetInt();
          if (x >= 0) sum += static_cast<uint64_t>(x);
        }
      }

      if (bValPtr) {
        const auto& bval = *bValPtr;
        if (bval.IsUint64()) {
          sum += bval.GetUint64();
        } else if (bval.IsUint()) {
          sum += static_cast<uint64_t>(bval.GetUint());
        } else if (bval.IsInt64()) {
          const int64_t x = bval.GetInt64();
          if (x >= 0) sum += static_cast<uint64_t>(x);
        } else if (bval.IsInt()) {
          const int x = bval.GetInt();
          if (x >= 0) sum += static_cast<uint64_t>(x);
        }
      }
    }
  }

  return sum;
}

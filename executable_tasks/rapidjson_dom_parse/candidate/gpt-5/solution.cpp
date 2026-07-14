#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  // Use iterative parsing for better performance and pass explicit length.
  doc.Parse<rapidjson::kParseIterativeFlag>(json_input.data(), json_input.size());

  uint64_t sum = 0;
  if (!doc.IsArray()) {
    return 0; // Defensive; baseline assumed array but this avoids UB on bad input
  }

  auto arr = doc.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const rapidjson::Value& obj = *it;

    uint64_t id = 0;
    uint64_t x = 0;
    uint64_t vals1 = 0;

    // Single pass over object members to avoid repeated member lookups
    for (auto m = obj.MemberBegin(); m != obj.MemberEnd(); ++m) {
      const char* key = m->name.GetString();
      const rapidjson::SizeType len = m->name.GetStringLength();
      switch (key[0]) {
        case 'i':
          if (len == 2 && key[1] == 'd') {
            id = m->value.GetUint64();
          }
          break;
        case 'x':
          if (len == 1) {
            x = m->value.GetUint64();
          }
          break;
        case 'v':
          if (len == 4 && key[1] == 'a' && key[2] == 'l' && key[3] == 's') {
            vals1 = m->value[1].GetUint64();
          }
          break;
        default:
          break;
      }
    }

    sum += id + x + vals1;
  }

  return sum;
}

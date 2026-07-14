#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  using namespace rapidjson;

  Document doc;
  doc.Parse<kParseIterativeFlag>(json_input.data(), json_input.size());
  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  const auto arr = doc.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const Value& item = *it;
    if (!item.IsObject()) continue;

    auto f = item.FindMember(StringRef("flag"));
    if (f == item.MemberEnd() || !f->value.IsBool() || !f->value.GetBool()) {
      continue;
    }

    auto idm = item.FindMember(StringRef("id"));
    auto vm = item.FindMember(StringRef("value"));
    auto nm = item.FindMember(StringRef("name"));

    // Assuming valid input as per task; these checks are minimal to avoid UB.
    if (idm == item.MemberEnd() || vm == item.MemberEnd() || nm == item.MemberEnd()) continue;
    if (!idm->value.IsUint64() || !vm->value.IsUint64() || !nm->value.IsString()) continue;

    const uint64_t id = idm->value.GetUint64();
    const uint64_t val = vm->value.GetUint64();
    const uint64_t nameLen = static_cast<uint64_t>(nm->value.GetStringLength());

    sum += id;
    sum += val;
    sum += nameLen;
  }

  return sum;
}

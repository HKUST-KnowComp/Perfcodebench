#include "interface.h"

#include <cstdint>
#include <string>
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  if (json_input.empty()) {
    return 0ULL;
  }

  // Make a mutable copy so we can use RapidJSON in-situ parsing to avoid
  // allocating/copying strings into the DOM.
  std::string buffer = json_input;

  rapidjson::Document doc;
  doc.ParseInsitu<rapidjson::kParseStopWhenDoneFlag>(&buffer[0]);
  if (doc.HasParseError() || !doc.IsArray()) {
    return 0ULL;
  }

  uint64_t sum = 0ULL;

  // Use StringRef to avoid computing string lengths repeatedly.
  static const rapidjson::StringRef idKey("id");
  static const rapidjson::StringRef xKey("x");
  static const rapidjson::StringRef valsKey("vals");

  auto arr = doc.GetArray();
  for (auto& v : arr) {
    auto obj = v.GetObject();

    // Assuming well-formed input per task contract for performance.
    const auto idIt = obj.FindMember(idKey);
    const auto xIt = obj.FindMember(xKey);
    const auto valsIt = obj.FindMember(valsKey);

    sum += idIt->value.GetUint64();
    sum += xIt->value.GetUint64();

    auto valsArr = valsIt->value.GetArray();
    sum += valsArr[1].GetUint64();
  }

  return sum;
}

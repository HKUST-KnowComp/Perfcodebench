#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  // Use in-situ parsing on a mutable buffer to minimize allocations/copies
  std::string buffer(json_input);
  rapidjson::Document doc;
  doc.ParseInsitu(buffer.data());

  uint64_t sum = 0;
  if (!doc.IsArray()) {
    return 0;
  }

  const rapidjson::StringRef kId("id");
  const rapidjson::StringRef kNested("nested");
  const rapidjson::StringRef kA("a");
  const rapidjson::StringRef kB("b");

  auto arr = doc.GetArray();
  const rapidjson::SizeType n = arr.Size();
  for (rapidjson::SizeType i = 0; i < n; ++i) {
    const rapidjson::Value& item = arr[i];
    const auto obj = item.GetObject();
    const uint64_t id = obj[kId].GetUint64();
    const auto nested = obj[kNested].GetObject();
    sum += id;
    sum += nested[kA].GetUint64();
    sum += nested[kB].GetUint64();
  }

  return sum;
}

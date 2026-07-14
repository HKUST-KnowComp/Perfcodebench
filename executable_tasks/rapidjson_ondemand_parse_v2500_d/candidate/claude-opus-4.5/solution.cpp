#include "interface.h"

#include <rapidjson/document.h>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
  // Copy input for in-situ parsing (modifies the buffer)
  std::vector<char> buffer(json_input.begin(), json_input.end());
  buffer.push_back('\0');
  
  rapidjson::Document doc;
  doc.ParseInsitu(buffer.data());
  
  uint64_t sum = 0;
  const auto& arr = doc.GetArray();
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const auto& item = arr[i];
    sum += item["id"].GetUint64();
    const auto& nested = item["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}
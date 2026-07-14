#include "interface.h"

#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace {

void make_inputs(
    std::vector<uint32_t>& ids,
    std::vector<uint32_t>& values,
    std::vector<std::string>& names,
    std::size_t count) {
  ids.clear();
  values.clear();
  names.clear();
  ids.reserve(count);
  values.reserve(count);
  names.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    ids.push_back(static_cast<uint32_t>(i));
    values.push_back(static_cast<uint32_t>(i * 29 + 7));
    std::ostringstream out;
    out << "item_" << (i % 257) << "_" << (i % 19);
    names.push_back(out.str());
  }
}

std::string expected_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  nlohmann::ordered_json arr = nlohmann::ordered_json::array();
  for (std::size_t i = 0; i < ids.size(); ++i) {
    nlohmann::ordered_json obj;
    obj["id"] = ids[i];
    obj["flag"] = ((ids[i] % 3U) == 0U);
    obj["value"] = values[i];
    obj["name"] = names[i];
    arr.push_back(std::move(obj));
  }
  return arr.dump();
}

}  // namespace

int main() {
  std::vector<uint32_t> ids;
  std::vector<uint32_t> values;
  std::vector<std::string> names;
    make_inputs(ids, values, names, 780000);
  const std::string expected = expected_json(ids, values, names);

  const auto start = std::chrono::steady_clock::now();
  const std::string actual = build_json(ids, values, names);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_size\":" << expected.size() << ","
            << "\"actual_size\":" << actual.size() << "}\n";
  return ok ? 0 : 1;
}

#include "interface.h"

#include <rapidjson/writer.h>

namespace {

// Minimal output stream for RapidJSON Writer that appends to std::string
struct StringAppender {
  using Ch = char;
  explicit StringAppender(std::string& out) : s(out) {}
  void Put(Ch c) { s.push_back(c); }
  void Flush() {}
  // Optional optimization used by Writer in some paths
  void PutN(Ch c, rapidjson::SizeType n) { s.append(static_cast<std::size_t>(n), c); }

  std::string& s;
};

inline std::size_t estimate_capacity(std::size_t n, std::size_t total_name_len) {
  if (n == 0) return 2; // "[]"
  // Rough per-object overhead without the name content: braces, commas, colons, quotes for keys,
  // boolean/number digits (padded generously), and quotes for the string value itself.
  // Keys overhead: {"id":, "flag":, "value":, "name":} with commas -> ~34 chars baseline
  // Add allowance for digits/bool/comma -> ~16
  const std::size_t per_obj_overhead = 50; // conservative
  // Array commas between objects: (n - 1)
  return 2 + (n * per_obj_overhead) + total_name_len + (n - 1);
}

} // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Precompute capacity
  std::size_t total_name_len = 0;
  for (const auto& nm : names) total_name_len += nm.size();

  std::string out;
  out.reserve(estimate_capacity(n, total_name_len));

  StringAppender app(out);
  rapidjson::Writer<StringAppender> writer(app);

  writer.StartArray();

  if (n) {
    // Predefined keys with known lengths to avoid strlen
    constexpr const char kId[] = "id";
    constexpr const char kFlag[] = "flag";
    constexpr const char kValue[] = "value";
    constexpr const char kName[] = "name";

    const rapidjson::SizeType kIdLen = static_cast<rapidjson::SizeType>(sizeof(kId) - 1);
    const rapidjson::SizeType kFlagLen = static_cast<rapidjson::SizeType>(sizeof(kFlag) - 1);
    const rapidjson::SizeType kValueLen = static_cast<rapidjson::SizeType>(sizeof(kValue) - 1);
    const rapidjson::SizeType kNameLen = static_cast<rapidjson::SizeType>(sizeof(kName) - 1);

    for (std::size_t i = 0; i < n; ++i) {
      writer.StartObject();

      writer.Key(kId, kIdLen);
      writer.Uint(ids[i]);

      writer.Key(kFlag, kFlagLen);
      writer.Bool((ids[i] % 3U) == 0U);

      writer.Key(kValue, kValueLen);
      writer.Uint(values[i]);

      writer.Key(kName, kNameLen);
      const std::string& nm = names[i];
      writer.String(nm.data(), static_cast<rapidjson::SizeType>(nm.size()));

      writer.EndObject();
    }
  }

  writer.EndArray();

  return out;
}

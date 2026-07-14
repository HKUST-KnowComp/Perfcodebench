#include "interface.h"

#include <rapidjson/writer.h>
#include <string>
#include <vector>

// Lightweight RapidJSON OutputStream that appends directly to a std::string
struct StringAppendStream {
  using Ch = char;
  explicit StringAppendStream(std::string& out) : s(out) {}
  void Put(Ch c) { s.push_back(c); }
  void Flush() {}
  // Allow Writer to efficiently output repeated characters
  void PutN(Ch c, size_t n) { s.append(n, c); }

private:
  std::string& s;
};

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  // Precompute an upper-bound-ish capacity to minimize reallocations.
  // Account for names, quotes/escapes, and per-object structural overhead.
  size_t names_len_sum = 0;
  for (const auto& name : names) names_len_sum += name.size();
  // Heuristic: per object ~48 bytes overhead + name length (with potential escaping, approximate *2)
  size_t reserve_cap = 2 + n * 48 + names_len_sum * 2; // 2 for []

  std::string out;
  out.reserve(reserve_cap);

  StringAppendStream os(out);
  rapidjson::Writer<StringAppendStream> w(os);

  w.StartArray();
  for (std::size_t i = 0; i < n; ++i) {
    w.StartObject();

    w.Key("id", 2);
    w.Uint(ids[i]);

    w.Key("flag", 4);
    w.Bool((ids[i] % 3U) == 0U);

    w.Key("value", 5);
    w.Uint(values[i]);

    w.Key("name", 4);
    const std::string& name = names[i];
    w.String(name.data(), static_cast<rapidjson::SizeType>(name.size()));

    w.EndObject();
  }
  w.EndArray();

  return out;
}

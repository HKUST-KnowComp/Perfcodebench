#include "interface.h"
#include "yyjson.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const size_t n = ids.size();
  // Fast path for empty input
  if (n == 0) {
    return "[]";
  }

  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  if (!doc) {
    // Fallback minimal result to ensure function returns a valid JSON
    return "[]";
  }

  yyjson_mut_val *root = yyjson_mut_arr(doc);
  if (!root) {
    yyjson_mut_doc_free(doc);
    return "[]";
  }
  yyjson_mut_doc_set_root(doc, root);

  // Constant key lengths to avoid strlen
  constexpr size_t k_id_len = 2;     // "id"
  constexpr size_t k_flag_len = 4;   // "flag"
  constexpr size_t k_value_len = 5;  // "value"
  constexpr size_t k_name_len = 4;   // "name"

  for (size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    if (!obj) {
      // On allocation failure, continue building best-effort array
      continue;
    }

    // id
    yyjson_mut_val *id_val = yyjson_mut_uint(doc, static_cast<uint64_t>(ids[i]));
    yyjson_mut_obj_addn(obj, "id", k_id_len, id_val);

    // flag
    bool flag = ((ids[i] % 3u) == 0u);
    yyjson_mut_val *flag_val = yyjson_mut_bool(doc, flag);
    yyjson_mut_obj_addn(obj, "flag", k_flag_len, flag_val);

    // value
    yyjson_mut_val *value_val = yyjson_mut_uint(doc, static_cast<uint64_t>(values[i]));
    yyjson_mut_obj_addn(obj, "value", k_value_len, value_val);

    // name
    const std::string &name = names[i];
    yyjson_mut_val *name_val = yyjson_mut_strncpy(doc, name.data(), name.size());
    yyjson_mut_obj_addn(obj, "name", k_name_len, name_val);

    yyjson_mut_arr_add(root, obj);
  }

  size_t len = 0;
  char *buf = yyjson_mut_write(doc, 0, nullptr, &len);

  std::string out;
  if (buf) {
    out.assign(buf, len);
    free(buf);
  } else {
    // Best-effort: if writing fails, at least return an empty array
    out = "[]";
  }

  yyjson_mut_doc_free(doc);
  return out;
}

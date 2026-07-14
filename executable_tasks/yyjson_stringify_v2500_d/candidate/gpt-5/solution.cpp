#include "interface.h"
#include "yyjson.h"

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  if (n == 0) {
    return "[]";
  }

  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  if (!doc) {
    // Fallback: extremely unlikely OOM; return empty array to avoid UB
    return "[]";
  }

  yyjson_mut_val *root = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, root);

  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);

    // Maintain field order: id, flag, value, name
    yyjson_mut_obj_add_uint(doc, obj, "id", static_cast<uint64_t>(ids[i]));
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3u) == 0u);
    yyjson_mut_obj_add_uint(doc, obj, "value", static_cast<uint64_t>(values[i]));
    yyjson_mut_obj_add_str(doc, obj, "name", names[i].c_str());

    yyjson_mut_arr_add_val(root, obj);
  }

  size_t len = 0;
  char *buf = yyjson_mut_write(doc, &len, 0); // 0 => minified, default escaping

  std::string out;
  if (buf) {
    out.assign(buf, len);
    free(buf);
  } else {
    // Fallback: should not happen, but keep consistent behavior
    out = "[]";
  }

  yyjson_mut_doc_free(doc);
  return out;
}

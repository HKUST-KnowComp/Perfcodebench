#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstdlib>

#include "yyjson.h"

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  if (!doc) {
    return std::string("[]");
  }

  yyjson_mut_val* root = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, root);

  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);

    yyjson_mut_obj_addn(obj, "id", 2, yyjson_mut_uint(doc, static_cast<uint64_t>(ids[i])));
    yyjson_mut_obj_addn(obj, "flag", 4, yyjson_mut_bool(doc, (ids[i] % 3u) == 0u));
    yyjson_mut_obj_addn(obj, "value", 5, yyjson_mut_uint(doc, static_cast<uint64_t>(values[i])));

    const std::string& name = names[i];
    yyjson_mut_obj_addn(obj, "name", 4,
                        yyjson_mut_strn(doc, name.data(), name.size()));

    yyjson_mut_arr_add(root, obj);
  }

  size_t len = 0;
  char* json = yyjson_mut_write(doc, 0, &len); // minified by default
  yyjson_mut_doc_free(doc);

  std::string out;
  if (json) {
    out.assign(json, len);
    std::free(json);
  }
  return out;
}

#include "interface.h"
#include <yyjson.h>
#include <string>
#include <vector>
#include <cmath>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const size_t n = ids.size();

  // Estimate upper-bound buffer size:
  // [{},] * n  -> 2*n
  // "id":<uint>, -> 6+10+1 = 17
  // "flag":true/false, -> 7+5 = 12
  // "value":<uint>, -> 8+10+1 = 19
  // "name":"<escaped>" -> 7+2*max_name_len
  size_t name_total = 0;
  for (const auto& s : names) name_total += s.size();
  size_t buf_cap = 2*n + n*(17+12+19) + 7*n + 2*name_total + 64;

  std::string out;
  out.resize(buf_cap);

  yyjson_alc alc;
  yyjson_alc_pool_init(&alc, nullptr, buf_cap);
  yyjson_mut_doc* doc = yyjson_mut_doc_new(&alc);
  yyjson_mut_val* root = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, root);

  for (size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);
    yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
    yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
    yyjson_mut_obj_add_strn(doc, obj, "name", names[i].data(), names[i].size());
    yyjson_mut_arr_append(root, obj);
  }

  size_t written = yyjson_mut_write_opts(doc, YYJSON_WRITE_NOFLAG, &alc, nullptr, nullptr);
  out.assign(alc.cur, written);

  yyjson_mut_doc_free(doc);
  yyjson_alc_pool_destroy(&alc);
  return out;
}
#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;
  yyjson_doc* doc = yyjson_parse(json_input.c_str(), NULL);
  if (doc == NULL) {
    return 0;
  }
  yyjson_iter iter;
  yyjson_iter_init(&iter, doc);
  while (yyjson_iter_next(&iter)) {
    if (iter.val->tag == YYJSON_TYPE_OBJ) {
      yyjson_val* id = yyjson_get_prop(iter.val, "id");      if (id != NULL && id->tag == YYJSON_TYPE_UINT) {
        sum += id->uint64;
      }
      yyjson_val* nested = yyjson_get_prop(iter.val, "nested");
      if (nested != NULL && nested->tag == YYJSON_TYPE_OBJ) {
        yyjson_val* a = yyjson_get_prop(nested, "a");
        if (a != NULL && a->tag == YYJSON_TYPE_UINT) {
          sum += a->uint64;
        }
        yyjson_val* b = yyjson_get_prop(nested, "b");
        if (b != NULL && b->tag == YYJSON_TYPE_UINT) {
          sum += b->uint64;
        }
      }
    }
  }
  yyjson_doc_free(doc);
  return sum;
}
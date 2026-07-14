#include "interface.h"

#include <rapidjson/document.h>
#include <cstring>
#include <stdexcept>

uint64_t compute_checksum(const std::string& json_input) {
    using namespace rapidjson;
    Document doc;
    char* buf = const_cast<char*>(json_input.data());
    if (doc.ParseInsitu(buf).HasParseError()) {
        throw std::runtime_error("JSON parse error");
    }

    uint64_t sum = 0;
    const Value& arr = doc.GetArray();
    for (SizeType i = 0; i < arr.Size(); ++i) {
        const Value& item = arr[i];
        bool flag = false;
        bool has_flag = false;
        uint64_t id = 0, value = 0, tag_len = 0;
        for (auto m = item.MemberBegin(); m != item.MemberEnd(); ++m) {
            const char* key = m->name.GetString();
            if (key[0] == 'f' && std::strcmp(key, "flag") == 0) {
                flag = m->value.GetBool();
                has_flag = true;
            } else if (key[0] == 'i' && std::strcmp(key, "id") == 0) {
                id = m->value.GetUint64();
            } else if (key[0] == 'v' && std::strcmp(key, "value") == 0) {
                value = m->value.GetUint64();
            } else if (key[0] == 't' && std::strcmp(key, "tag") == 0) {
                tag_len = m->value.GetStringLength();
            }
        }
        if (has_flag && flag) {
            sum += id + value + tag_len;
        }
    }
    return sum;
}